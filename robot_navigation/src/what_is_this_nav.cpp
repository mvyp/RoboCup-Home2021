#include <ros/ros.h> 
#include <move_base_msgs/MoveBaseAction.h> 
#include <actionlib/client/simple_action_client.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"    
#include "std_msgs/String.h"
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>

using namespace std;
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; 
 
// set coordinates of goals
typedef struct _POSE
{
  double X;
  double Y;
  double cita;
} POSE;
POSE home= {3.571, -0.114, 3.14}; //迎接客人点

POSE kitchen= {3.432,1.17,2.982};
POSE dining_room={7.85,4.74,0.039};
POSE living_room={3.227,1.17,0};
POSE corridor={6.86,2.27,3.58};
POSE bed_room={9.783,0.35,-1.57};

POSE door={1.990,-0.016,-3.102}; //entrance
POSE bookcase={1.536,1.940,1.612};
POSE bed={10.498,1.052,-0.052};
POSE desk={10.428 ,0.196, -1.612};
POSE dining_table={9.766 ,3.858, 1.601};
POSE cupboard={5.156,5.530,1.591};
POSE dishwasher={4.292,5.541, 1.605};
POSE sink={3.476,5.561,1.568};
POSE counter={3.455, 4.047, -1.570};
POSE storange_table={1.376 ,4.281, -1.557};

POSE sofa={3.571, -0.114, 1.125};

POSE sofa1={3.57, -0.114, 0.603};//0.681
POSE sofa2={3.57, -0.114, 0.844};//1.807
POSE sofa3={3.57, -0.114, 1.125}; //
POSE sofa4={3.57, -0.114, 1.480};  

// set home (Remeber to change omni)
void setHome( ros::Publisher pub)
{
  geometry_msgs::PoseWithCovarianceStamped msg_poseinit;
  msg_poseinit.header.frame_id = "map";
  msg_poseinit.header.stamp = ros::Time::now();
  msg_poseinit.pose.pose.position.x =3.57;
  msg_poseinit.pose.pose.position.y = -0.114;
  msg_poseinit.pose.pose.orientation.w=0.000;
  msg_poseinit.pose.pose.orientation.x=0.000;
  msg_poseinit.pose.pose.orientation.y=0.000;
  msg_poseinit.pose.pose.orientation.z=1.000;
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

  }
  else ROS_WARN("tracking thread unexpected ERROR!");
}


int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "what_is_this_nav"); 
  ros::NodeHandle nh;
  ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);

  setHome(pub_initialpose);
  
  while(ros::ok())
  {
    ros::spinOnce();
    // rate_loop.sleep();
  }
  return 0; 
}  
