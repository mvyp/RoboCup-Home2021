//
// Created by sie-rc on 5/9/21.
//

#include "detect_ctl_head.h"

YOLO_GRAB::YOLO_GRAB() {
    vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 100);

    grab_start_flag_pub = nh.advertise<std_msgs::Bool>("/grab_start_flag", 10);

    boxes_sub = nh.subscribe<darknet_ros_msgs::BoundingBoxes>("/darknet_ros/bounding_boxes", 100, &YOLO_GRAB::yolo_callback, this);

    // 目标抓取物
    target_class = "milk";

    set_classes_threshold();
}

void YOLO_GRAB::yolo_callback(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg) {
    for(const auto& detect_box : msg->bounding_boxes){
        string obj_class = detect_box.Class;
        double prob = detect_box.probability;
        long long x_mid = (detect_box.xmin + detect_box.xmax) / 2;
        // long long y_mid = (detect_box.y_min + detect_box.y_max) / 2;
        long long x_length = detect_box.xmax - detect_box.xmin;
        // long long y_length = detect_box.y_max - detect_box.y_min;
        ROS_INFO("%s : prob = %f x_mid = %lld x_length = %lld\n", detect_box.Class.c_str(), detect_box.probability, x_mid, x_length);

        if(obj_class == target_class && prob >= 0.40 && STOP_PUB_VEL_FLAG){
            CLASS_THRESHOLD target_thres = classes_thres_map[target_class];
            if((x_mid < half_x_correct - 3 || x_mid > half_x_correct + 3) || (x_length < target_thres.x_threshold - target_thres.trust_bias_length || x_length > target_thres.x_threshold + target_thres.trust_bias_length)) {
                ROS_INFO("start to pub vel in front of %s", obj_class.c_str());
                geometry_msgs::Twist start_vel;
                start_vel.linear.y = (double)(half_x - x_mid) / (double)classes_thres_map[target_class].vel_lin_y_scale;
                start_vel.linear.x = (double)(classes_thres_map[target_class].x_threshold - x_length) / (double)classes_thres_map[target_class].vel_lin_x_scale;
                vel_pub.publish(start_vel);
            }
            else{
                ROS_INFO("stop publishing vel");
                geometry_msgs::Twist stop_vel;
                stop_vel.linear.x = 0;
                stop_vel.linear.y = 0;
                vel_pub.publish(stop_vel);

                std_msgs::Bool Grab_Flag;
                Grab_Flag.data = true;
                grab_start_flag_pub.publish(Grab_Flag);
                STOP_PUB_VEL_FLAG = false;
            }
        }
    }
}

void YOLO_GRAB::set_classes_threshold() {
    // 建立不同类别的yolo框改底盘速度对准
    CLASS_THRESHOLD milk_thres;
    milk_thres.vel_lin_x_scale = 200;
    milk_thres.vel_lin_y_scale = 4000;
    milk_thres.x_threshold = 95;
    milk_thres.trust_bias_length = 5;
    classes_thres_map["milk"] = milk_thres;

    CLASS_THRESHOLD water_thres;
    water_thres.vel_lin_x_scale = 200;
    water_thres.vel_lin_y_scale = 4000;
    water_thres.x_threshold = 95;
    water_thres.trust_bias_length = 20;
    classes_thres_map["water"] = water_thres;
}
