#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <sensor_msgs/PointCloud2.h>
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/console/parse.h>
#include <pcl_ros/point_cloud.h>
#include <pcl_ros/transforms.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>

#include <pcl/filters/passthrough.h>

#include <pcl/segmentation/supervoxel_clustering.h>
#include <pcl/segmentation/lccp_segmentation.h>

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#define MODE_NO_BAG 0
#define MODE_DETECT_BAG 1
static int nMode = MODE_NO_BAG;

using namespace std;

typedef pcl::PointCloud<pcl::PointXYZRGBA> PointCloud;
static std::string pc_topic;
static tf::TransformListener *tf_listener;

void Segment_PointCloud_CB(const sensor_msgs::PointCloud2 &input){
    if(nMode == MODE_NO_BAG){
        return;
    }
    sensor_msgs::PointCloud2 pc_footprint;
    bool res = tf_listener->waitForTransform("/base_footprint", input.header.frame_id, input.header.stamp, ros::Duration(5.0)); // 求当前点云帧对应时序信息的/base_footprint
    if(!res)
    {
        return;
    }
    pcl_ros::transformPointCloud("/base_footprint", input, pc_footprint, *tf_listener); //把相对于kinect的input点云帧通过tf变换到/base_footprint下，存入pc_footprint

    // source cloud
    PointCloud cloud_src;
    pcl::fromROSMsg(pc_footprint, cloud_src); // cloud_src是相对于底盘的点云位置从ROS版本->PCL版本
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud_source_ptr;
    cloud_source_ptr = cloud_src.makeShared();

//    pcl::PassThrough<pcl::PointXYZRGBA> pass;//设置滤波器对象
//    pass.setInputCloud (cloud_source_ptr);
//    pass.setFilterFieldName ("z");
//    pass.setFilterLimits (0.3, 1.5);
//    pass.filter (*cloud_source_ptr);
//    pass.setFilterFieldName ("x");
//    pass.setFilterLimits (0.0, 1.5);
//    pass.filter (*cloud_source_ptr);
//    pass.setFilterFieldName ("y");
//    pass.setFilterLimits (-1.5, 1.5);
//    pass.filter (*cloud_source_ptr);

}

void ChangeModeCB(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg){
    for(const auto& detect_box : msg->bounding_boxes){
        string obj_class = detect_box.Class;
        long long x_length = detect_box.xmax - detect_box.xmin;
        long long bag_size = 160;

        if(obj_class == "paper-bag" && x_length >= bag_size){
            nMode = MODE_DETECT_BAG;
            ROS_INFO("Start to Segment PointCloud of the Bag.");
        }
    }
}

int main(int argc, char** argv){
    ros::init(argc, argv, "bag_segmentation");
    ROS_INFO("Bag Segmentation Start!");
    tf_listener = new tf::TransformListener();

    ros::NodeHandle nh_param("~");
    nh_param.param<std::string>("qc_topic", pc_topic, "/kinect2/qhd/points");
    // 可以在launch里用<param name="qc_topic" value="/kinect2/sd/points"/>

    ros::NodeHandle nh;
    ros::Subscriber pc_sub = nh.subscribe(pc_topic, 10, Segment_PointCloud_CB);
    ros::Subscriber bag_sub = nh.subscribe("/darknet_ros/bounding_boxes", 10, ChangeModeCB);

    ros::spin();

    delete tf_listener;

    return 0;
}
