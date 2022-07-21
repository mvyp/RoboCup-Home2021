/***************************************************************
 * @author FengYixiao
 **************************************************************/

#ifndef SRC_CARRY_LUGGAGE_H_H
#define SRC_CARRY_LUGGAGE_H_H

#include <ros/ros.h>
#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <tf/transform_broadcaster.h>
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <iostream>

#define STEP_DETECT_PERSON 0
#define STEP_LIFT_UP_ARM   1
#define STEP_TIGHT_HOLD    2
#define STEP_FOLLOW_START  3
#define STEP_ARM_DOWN      4
#define STEP_LOOSEN_GRAB   5
#define STEP_ARM_BACK      6
#define STEP_NAVIGATION    7
#define STEP_DONE          8
#define STEP_ARM_ON        9

using namespace std;

class Carry_Luggage{
public:
    Carry_Luggage();

    void darknetCB(const darknet_ros_msgs::BoundingBoxes::ConstPtr &input);

    void follow_stop_fromvoice_CB(const std_msgs::String::ConstPtr &input);

    void done_CB(const std_msgs::String::ConstPtr &input);

    void Lift_up_Arm();

private:
    ros::NodeHandle nh;

    // 检测是否有人在摄像头前出现过
    ros::Subscriber darknet_sub;

    // ros::Rate loop_rate = ros::Rate(0.08); // 语音播放卡顿
    ros::Rate r = ros::Rate(30);

    // 控制流程
    int nStep = STEP_DETECT_PERSON;

    // 机械臂部分
    int nTimeDelayCounter = 0;
    float grab_gripper_value = 0.0f;
    sensor_msgs::JointState mani_ctrl_msg;
    ros::Publisher mani_ctrl_pub;

    // 语音部分
    std_msgs::String phase_first_msg; // 把袋子放夹爪
    std_msgs::String phase_second_msg; // 夹爪抓住后提示跟随
    // std_msgs::String phase_third_msg; // 跟随时说话
    std_msgs::String phase_fourth_msg; // 放下物品提示
    std_msgs::String phase_fifth_msg;
    std_msgs::String phase_sixth_msg; // 导航返回提示
    std_msgs::String phase_seventh_msg; // 结束任务提示
    std_msgs::String phase_eighth_msg;
    std_msgs::String phase_wait_msg;
    bool wake_up_flag;
    ros::Subscriber follow_stop_sub; // 接收
    ros::Subscriber wake_up_sub;     // 接收wakeup停止说“等等我”
    ros::Publisher voice_pub;

    // 跟随部分
    std_msgs::String follow_flag;
    bool limit_follow;
    ros::Publisher follow_flag_pub;

    // 导航部分
    std_msgs::Bool nav_flag;
    ros::Publisher nav_pub;
    ros::Subscriber done_sub;
};

#endif //SRC_CARRY_LUGGAGE_H_H
