//
// Created by sie-rc on 5/9/21.
//

#ifndef POSE_DETECT_DETECT_CTL_HEAD_H
#define POSE_DETECT_DETECT_CTL_HEAD_H

#include "ros/ros.h"
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <iostream>
#include <cstring>
#include <map>

using namespace std;

typedef struct threshold_flag{
    long long vel_lin_x_scale; // linear.x 速度的缩小scale
    long long vel_lin_y_scale; // linear.y 速度的缩小scale
    long long x_threshold; // 用来确定识别框x_length的稳定大小
    long long trust_bias_length; // 用来确定物品框x_length的允许的波动程度
} CLASS_THRESHOLD;

class YOLO_GRAB{
public:
    YOLO_GRAB();

    void yolo_callback(const darknet_ros_msgs::BoundingBoxes::ConstPtr &msg);

    void set_classes_threshold();

private:
    ros::NodeHandle nh;

    //订阅检测框
    ros::Subscriber boxes_sub;

    //按照检测框发布速度
    ros::Publisher vel_pub;

    //发布允许点云数据传给机械臂抓取节点的flag
    ros::Publisher grab_start_flag_pub;

    string target_class; // 目标抓取物
    map<string, CLASS_THRESHOLD> classes_thres_map; // 用来放不同物体识别框的调参

    bool STOP_PUB_VEL_FLAG = true; // 如果没有稳定到信赖的识别框大小，就持续调整

    // 图片分辨率
    long long full_x = 1920; // kinect/hd
    long long full_y = 1080;
    long long half_x = full_x / 2;
    long long half_y = full_y / 2;

    // color的单目相机在kinect右边，bias用来修正
    long long camera_len_bias = 20; // ***要改
    long long half_x_correct = half_x - camera_len_bias;
};

#endif //POSE_DETECT_DETECT_CTL_HEAD_H
