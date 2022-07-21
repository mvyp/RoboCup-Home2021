/***************************************************************
 * @author FengYixiao
 **************************************************************/

#include "carry_luggage_h.h"

Carry_Luggage::Carry_Luggage() {
    darknet_sub = nh.subscribe<darknet_ros_msgs::BoundingBoxes>("/darknet_ros/bounding_boxes", 100, &Carry_Luggage::darknetCB, this);

    voice_pub = nh.advertise<std_msgs::String>("/voiceWords1", 100);

    mani_ctrl_pub = nh.advertise<sensor_msgs::JointState>("/wpb_home/mani_ctrl", 30);

    follow_flag_pub = nh.advertise<std_msgs::String>("/follow_flag", 100);
    follow_stop_sub = nh.subscribe<std_msgs::String>("/follow_stop", 100, &Carry_Luggage::follow_stop_fromvoice_CB, this);

    nav_pub = nh.advertise<std_msgs::Bool>("/back_to_home", 1);
    done_sub = nh.subscribe<std_msgs::String>("/carry_done", 10, &Carry_Luggage::done_CB, this);

    limit_follow = true;

    phase_first_msg.data = "Please help me put the luggage between my hand."; // 请帮我把袋子放在夹爪间
    phase_second_msg.data = "I will follow you. When reaching the car, please speak, hello friday, to me.";
    // 请站在我前面一米左右，我会跟着你，通过门的时候请保持在我视野内，到达目的地请告诉我 When passing through the door, please keep in my view.
    // phase_third_msg.data = "请等等我，我腿短";
    phase_fourth_msg.data = "I will put the luggage down."; // 我将放下袋子
    phase_fifth_msg.data = "Please wait for me to take back the arm."; // 请等待我回收机械臂
    phase_sixth_msg.data = "I am going back to the, arina."; // 我在返回竞技场
    phase_seventh_msg.data = "I have finished.";
    phase_wait_msg.data = "Please wait a moment."; // 请稍后

    // 机械臂参数
    mani_ctrl_msg.name.resize(2);
    mani_ctrl_msg.position.resize(2);
    mani_ctrl_msg.velocity.resize(2);
    mani_ctrl_msg.name[0] = "lift";
    mani_ctrl_msg.name[1] = "gripper";
    mani_ctrl_msg.position[0] = 0;
    mani_ctrl_msg.velocity[0] = 0.5;     //升降速度(单位:米/秒)
    mani_ctrl_msg.position[1] = 0.16;
    mani_ctrl_msg.velocity[1] = 5;       //手爪开合角速度(单位:度/秒)

    Lift_up_Arm();
}

void Carry_Luggage::darknetCB(const darknet_ros_msgs::BoundingBoxes::ConstPtr &input) {
    for(auto detect_box : input->bounding_boxes){
        if(detect_box.Class == "human" && nStep == STEP_DETECT_PERSON){
            ROS_WARN("[DETECT_PERSON]");
            nStep = STEP_LIFT_UP_ARM;
        }
    }
}

void Carry_Luggage::follow_stop_fromvoice_CB(const std_msgs::String::ConstPtr &input) { // phase4 停止跟随
    limit_follow = false;
    if(input->data == "stop" && nStep == STEP_FOLLOW_START){
        ROS_WARN("[FOLLOW_STOP]");
        follow_flag.data = "0";
        follow_flag_pub.publish(follow_flag);
        ROS_WARN("[FOLLOW_STOP]->[START_PUT_DOWN_LUGGAGE]");
        nStep = STEP_ARM_DOWN;
    }
}

void Carry_Luggage::done_CB(const std_msgs::String::ConstPtr &input) {
    if(input->data == "done" && nStep == STEP_NAVIGATION){
        ROS_WARN("[MISSION_FINISH]");
        nStep = STEP_DONE;
    }
}

void Carry_Luggage::Lift_up_Arm() {
    while(nh.ok()){
        if(nStep == STEP_DETECT_PERSON){
            ROS_WARN("[PLEASE WAIT]");
            voice_pub.publish(phase_wait_msg);
            sleep(4);
        }

        if(nStep == STEP_LIFT_UP_ARM){ // phase1 抬高机械臂
            if(nTimeDelayCounter == 0){
                mani_ctrl_msg.position[0] = 1.0; // 改动项，机械臂抬高高度
                mani_ctrl_msg.position[1] = 0.16;
                mani_ctrl_pub.publish(mani_ctrl_msg);
                ROS_WARN("[STEP_LIFT_UP_ARM] lift= %.2f gripper= %.2f", mani_ctrl_msg.position[0], mani_ctrl_msg.position[1]);
            }
            nTimeDelayCounter++;
            mani_ctrl_pub.publish(mani_ctrl_msg);
            if(nTimeDelayCounter > 20*30){
                nTimeDelayCounter = 0;
                voice_pub.publish(phase_first_msg); // 说：“请帮我把袋子放在夹爪间”
                sleep(10);
                nStep = STEP_TIGHT_HOLD;
            }
        }

        if(nStep == STEP_TIGHT_HOLD){ // phase2 夹紧袋子
            if(nTimeDelayCounter == 0){
                ROS_WARN("[STEP_TIGHT_HOLD] gripper= %.2f", grab_gripper_value);
            }
            mani_ctrl_msg.position[1] = grab_gripper_value;
            mani_ctrl_pub.publish(mani_ctrl_msg);
            nTimeDelayCounter++;
            if(nTimeDelayCounter > 8*30){
                nTimeDelayCounter = 0;
                voice_pub.publish(phase_second_msg);
                /*
                 * 说：“请站在我前面一米左右，我会跟着你，通过门的时候请保持在我视野内，到达目的地请告诉我”
                 */
                sleep(12);
                ROS_WARN("[GRAB_SUCCESS]");
                nStep = STEP_FOLLOW_START;
            }
        }

        if(nStep == STEP_FOLLOW_START){ // phase3 开始跟随
            if(limit_follow) {
                follow_flag.data = "1";
                follow_flag_pub.publish(follow_flag);
            }
        }

        if(nStep == STEP_ARM_DOWN){ // phase5 机械臂放下
            if(nTimeDelayCounter == 0){
                mani_ctrl_msg.position[0] = 0.5;
                mani_ctrl_pub.publish(mani_ctrl_msg);
            }
            nTimeDelayCounter++;
            mani_ctrl_pub.publish(mani_ctrl_msg);
            if(nTimeDelayCounter > 15*30){
                nTimeDelayCounter = 0;
                ROS_WARN("[ARM_DOWN]");
                nStep = STEP_LOOSEN_GRAB;
            }
        }

        if(nStep == STEP_LOOSEN_GRAB){ // phase6 松爪
            if(nTimeDelayCounter == 0){
                // voice_pub.publish(phase_fourth_msg); // 说：“我将放下袋子”
                // sleep(2);
                mani_ctrl_msg.position[1] = 0.16;
                mani_ctrl_pub.publish(mani_ctrl_msg);
            }
            nTimeDelayCounter++;
            mani_ctrl_pub.publish(mani_ctrl_msg);
            if(nTimeDelayCounter > 6*30){
                nTimeDelayCounter = 0;
                ROS_WARN("[LUGGAGE_DOWN]");
                nStep = STEP_ARM_BACK;
            }
        }

        if(nStep == STEP_ARM_BACK){
            if(nTimeDelayCounter == 0){
                // voice_pub.publish(phase_fifth_msg); // 说：“请等待我回收机械臂”
                // sleep(3);
                mani_ctrl_msg.position[0] = 0.0;
                mani_ctrl_pub.publish(mani_ctrl_msg);
            }
            nTimeDelayCounter++;
            mani_ctrl_pub.publish(mani_ctrl_msg);
            if(nTimeDelayCounter > 20*30){
                nTimeDelayCounter = 0;
                ROS_WARN("[ARM_BACK]");
                nStep = STEP_NAVIGATION;
            }
        }

        if(nStep == STEP_NAVIGATION){ // phase7 导航回竞技场
            nav_flag.data = true;
            voice_pub.publish(phase_sixth_msg); // 说：“我在返回竞技场”
            sleep(8);
            nav_pub.publish(nav_flag);
        }

        if(nStep == STEP_DONE){ // phase8 任务结束
			bool count = true;
			while(count){
		        	voice_pub.publish(phase_seventh_msg); // 说：“I have finished”
		        	sleep(8);
		        	ROS_WARN("[DONE]");
				count=false;
			}
        }
        ros::spinOnce();
        r.sleep();
    }
}
