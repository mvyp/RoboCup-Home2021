#include <ros/ros.h> 
#include <sensor_msgs/JointState.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <iostream>
#include <cstring>

using namespace std;

#define WAIT           0
#define ARM_UP         1
#define VOICE_SPEAK    2
#define ARM_DOWN       3
#define GO_DOOR        4

static int nStep = WAIT;

static ros::Publisher mani_ctrl_pub;
static sensor_msgs::JointState mani_ctrl_msg;

static int nTimeDelayCounter = 0;
int count_flag = 1;

void Callback(const std_msgs::String::ConstPtr &msg)
{
  if(msg->data == "arm_go" || msg->data == "arm_goarm_go")
  {
    nStep = ARM_UP;
  }
} 

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "arm_action"); 
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe("/finish_seat",1000 ,Callback);
  ros::Publisher voice_pub = nh.advertise<std_msgs::String>("/voiceWords1", 100);
  std_msgs::String voice_msg;

  stringstream ss1;
  stringstream ss2;

  mani_ctrl_pub = nh.advertise<sensor_msgs::JointState>("/wpb_home/mani_ctrl", 30);
  ros::Publisher back_door_pub = nh.advertise<std_msgs::String>("arm_finish", 10);
  
  mani_ctrl_msg.name.resize(2);
  mani_ctrl_msg.position.resize(2);
  mani_ctrl_msg.velocity.resize(2);
  mani_ctrl_msg.name[0] = "lift";
  mani_ctrl_msg.name[1] = "gripper";
  mani_ctrl_msg.position[0] = 0;
  mani_ctrl_msg.velocity[0] = 0.5;     //升降速度(单位:米/秒)
  mani_ctrl_msg.position[1] = 0.16;
  mani_ctrl_msg.velocity[1] = 5;       //手爪开合角速度(单位:度/秒)

  ros::Rate r(30);
  while(ros::ok()){
    if(nStep == ARM_UP){
      if(nTimeDelayCounter == 0){
        mani_ctrl_msg.position[0] = 0.5;
        mani_ctrl_msg.position[1] = 0.0;
        mani_ctrl_pub.publish(mani_ctrl_msg);
        ROS_WARN("[ARM UP] lift= %.2f  gripper= %.2f " ,mani_ctrl_msg.position[0], mani_ctrl_msg.position[1]);
      }
      nTimeDelayCounter++;
      mani_ctrl_pub.publish(mani_ctrl_msg);
      if(nTimeDelayCounter >= 10*30){
        nTimeDelayCounter = 0;
        ROS_INFO("[ARM UP]");
        nStep = VOICE_SPEAK;
      }
    }
    if(nStep == VOICE_SPEAK){
      stringstream ss;
      ss << "This is your seat. Please sit down.";
      voice_msg.data = ss.str();
      voice_pub.publish(voice_msg);
      sleep(4);
      nStep = ARM_DOWN;
    }
    if(nStep == ARM_DOWN){
      if(nTimeDelayCounter == 0){
        mani_ctrl_msg.position[0] = 0;
        mani_ctrl_msg.position[1] = 0.16;
        mani_ctrl_pub.publish(mani_ctrl_msg);
        ROS_WARN("[ARM DOWN] lift= %.2f  gripper= %.2f " ,mani_ctrl_msg.position[0], mani_ctrl_msg.position[1]);
      }
      nTimeDelayCounter++;
      mani_ctrl_pub.publish(mani_ctrl_msg);
      if(nTimeDelayCounter >= 10*30){
        nTimeDelayCounter = 0;
        ROS_INFO("[ARM DOWN]");
        nStep = GO_DOOR;
      }
    }
    if(nStep == GO_DOOR){
      if(count_flag == 1){
        ss1 << "arm_done";
        std_msgs::String back_door;
        back_door.data = ss1.str();
        back_door_pub.publish(back_door);
        count_flag++;
      }
    }
    ros::spinOnce();
    r.sleep();
  }
  
  return 0; 
}
