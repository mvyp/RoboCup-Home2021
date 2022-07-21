#include <ros/ros.h> 
#include <move_base_msgs/MoveBaseAction.h> 
#include <actionlib/client/simple_action_client.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"    
#include "std_msgs/String.h"
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <sstream>

using namespace std;
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; 

ros::Publisher start_grab;
std_msgs::String start_grab_msg ;

bool finish_grab_flag =0;

// set coordinates of goals
typedef struct _POSE
{
  double X;
  double Y;
  double cita;
} POSE;
POSE home= {6.037,-2.615,1.509};
POSE pose2= {5.996,1.082,0.000};
POSE pose1= {5.341, 2.341, 1.415};


// set home (Remeber to change omni)
void setHome(ros::Publisher pub)
{
  geometry_msgs::PoseWithCovarianceStamped msg_poseinit;
  msg_poseinit.header.frame_id = "map";
  msg_poseinit.header.stamp = ros::Time::now();
  msg_poseinit.pose.pose.position.x =  6.037;
  msg_poseinit.pose.pose.position.y =-2.615;
  msg_poseinit.pose.pose.orientation.w=0.728;
  msg_poseinit.pose.pose.orientation.x=0.000;
  msg_poseinit.pose.pose.orientation.y=0.000;
  msg_poseinit.pose.pose.orientation.z=0.685;
  pub.publish(msg_poseinit);
  ros::Duration(1.0).sleep();
  pub.publish(msg_poseinit);
  ros::Duration(1.0).sleep();
  pub.publish(msg_poseinit);
  ros::Duration(1.0).sleep();
 }

// set goal
void setGoal(POSE pose)
{
  MoveBaseClient ac("move_base", true); 
  while(!ac.waitForServer(ros::Duration(5.0))){ 
    ROS_WARN("Waiting for the move_base action server to come up"); 
  } 

  move_base_msgs::MoveBaseGoal goal; 
  tf::Quaternion q;
  goal.target_pose.header.frame_id = "map"; 
  goal.target_pose.header.stamp = ros::Time::now();
  goal.target_pose.pose.position.x = pose.X;
  goal.target_pose.pose.position.y = pose.Y; 
  q.setRPY( 0, 0, pose.cita);
  goal.target_pose.pose.orientation.x = q.x();
  goal.target_pose.pose.orientation.y = q.y();
  goal.target_pose.pose.orientation.z = q.z();
  goal.target_pose.pose.orientation.w = q.w();

  ROS_INFO("Sending goal"); 
  ac.sendGoal(goal); 
  ac.waitForResult();
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
    ROS_INFO("Succeeded"); 
    start_grab.publish(start_grab_msg);
  }
  else ROS_WARN("tracking thread unexpected ERROR!");
}

void StatusCallback(const std_msgs::String::ConstPtr& msg_p){
    ROS_INFO("The status of grab procession is:%s",msg_p->data.c_str());
    if(msg_p->data=="STEP_DONE")
        finish_grab_flag==1;
}

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "boxes_nav"); 
  ros::NodeHandle nh;
  ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);
  start_grab_msg.data = "0";
  ros::Publisher start_grab = nh.advertise<std_msgs::String>("/start_grab", 10);
  ros::Subscriber finish_grab = nh.subscribe("/wpb_home/grab_result", 30,StatusCallback);

  setHome(pub_initialpose);

  setGoal(pose1);
  if(finish_grab_flag) 
    setGoal(pose2);

  ros::spin();
  return 0; 
}  
