#include "ros/ros.h"
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <iostream>
#include <cstring>
#include <map>

using namespace std;
ros::Publisher voice_pub;

map<string, string> mapItem;

bool action_flag = false;

void yolo_callback(const darknet_ros_msgs::BoundingBoxes::ConstPtr &msg);

void start_detect_CB(const std_msgs::String::ConstPtr &input);

int main(int argc, char** argv){
    ros::init(argc,argv,"what_is_this_node");
    ros::NodeHandle nh;

    mapItem["apple"] = "This is apple. Its color is , red.";
    mapItem["chocolate drink"] = "This is chocolate drink. Its color is , blue and white";
    mapItem["coke"] = "This is coke. Its color is , red and black";
    mapItem["cracker"] = "This is cracker. Its shape is a , cuboid";
    mapItem["grape juice"] = "This is grape juice. Its color is , dark purple";
    mapItem["milk"] = "This is milk. Its color is , green and whit";
    mapItem["noodles"] = "This is noodles. Its shape is a , bowl";
    mapItem["orange"] = "This is orange. Its color is , orange";
    mapItem["orange juice"] = "This is orange juice. Its colors is , orange and pink";
    mapItem["photato chips"] = "This is photato chips. Its shape is , cylindrical";
    mapItem["pringle"] = "This is pringle. Its shape is an , octagonal column";
    mapItem["sausage"] = "This is sausage. Its color is , pink";
    mapItem["sprite"] = "This is sprite. Its color is , green and white";
    mapItem["human"] = "";
    mapItem["tray"] = "";
    mapItem["bag"] = "";
    mapItem["fork"] = "";
    mapItem["knife"] = "";
    mapItem["spoon"] = "";
    mapItem["dish"] = "";
    mapItem["bowl"] = "";

    ros::Subscriber yolo_sub = nh.subscribe("/darknet_ros/bounding_boxes", 10, yolo_callback);
    ros::Subscriber start_sub = nh.subscribe("/yolo_start", 1, start_detect_CB);
    voice_pub = nh.advertise<std_msgs::String>("/voiceWords1", 10);

    return 0;
}

void yolo_callback(const darknet_ros_msgs::BoundingBoxes::ConstPtr &msg){
    for(const auto& detect_box : msg->bounding_boxes){
        string class_name = detect_box.Class;
        double prob = detect_box.probability;
        long long x_mid = (detect_box.xmin + detect_box.xmax) / 2; // x方向质心
        // long long y_mid = (detect_box.y_min + detect_box.y_max) / 2;
        long long x_length = detect_box.xmax - detect_box.xmin;
        // long long y_length = detect_box.y_max - detect_box.y_min;
        ROS_INFO("%s : prob = %f x_mid = %lld x_length = %lld\n", detect_box.Class.c_str(), detect_box.probability, x_mid, x_length);

        if(action_flag && class_name != "human"){
            stringstream ss ,ss1;
            ss << mapItem[class_name];
            std_msgs::String voice_msg1;
            voice_msg1.data = ss.str();
            voice_pub.publish(voice_msg1);
            sleep(10);
            ss1 << "Please go to anothor please";
            std_msgs::String voice_msg2;
            voice_msg2.data = ss.str();
            voice_pub.publish(voice_msg2);
            action_flag = false;
        }
    }
}

void start_detect_CB(const std_msgs::String::ConstPtr &input){
    if(input->data == "detect_go"){
        action_flag = true;
        stringstream ss;
        ss << "Please put the target object in front of my camera.";
        std_msgs::String voice_msg;
        voice_msg.data = ss.str();
        voice_pub.publish(voice_msg);
    }
}
