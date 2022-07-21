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
POSE home= {0,0,0};  //门外起点
POSE door= {1,0, 3.14}; //门口面对门

bool global_flag = 1; 
int pose_flag = 0;
int count_flag = 0;   //  target sign 

// set home (Remeber to change omni)
void setHome(ros::Publisher pub)
{
  geometry_msgs::PoseWithCovarianceStamped msg_poseinit;
  msg_poseinit.header.frame_id = "map";
  msg_poseinit.header.stamp = ros::Time::now();
  msg_poseinit.pose.pose.position.x = 0;
  msg_poseinit.pose.pose.position.y = 0;
  msg_poseinit.pose.pose.orientation.w=1; //cos cita/2
  msg_poseinit.pose.pose.orientation.x=0.000;
  msg_poseinit.pose.pose.orientation.y=0.000;
  msg_poseinit.pose.pose.orientation.z=-0.005; //cos cita/2
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
  global_flag = 0;

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
    count_flag++;
    global_flag = 1;
  }
  else ROS_WARN("tracking thread unexpected ERROR!");
}

// set which goal
void setWhichGoal(){
  // if(count_flag == 0)
  //   setGoal(pose1);
  // else if(count_flag == 1)
  //   setGoal(pose2);
  // else if(count_flag == 2)
  //   setGoal(home);
  // else if(count_flag == 3)
  //   ROS_INFO("Get the end position.");
  // else
  //   ROS_ERROR("Count_flag is error!");
}

// callback function
void domsg(const std_msgs::String::ConstPtr &msg)
{
    // ROS_INFO("Got follow_flag = %s",msg->data.c_str());
    if(msg->data=="1")
        pose_flag = 1;
    else if(msg->data=="0")
        pose_flag = 0;
    else if(msg->data=="2")
        pose_flag = 2;
    else
        ROS_WARN("The message of ‘follow_flag’ is error!");
} 

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "base_pose_control"); 
  ros::NodeHandle nh;
  ros::Subscriber flag_sub = nh.subscribe("follow_flag",10, domsg);
  ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);

  setHome(pub_initialpose);

  setGoal(door);
  
  while(ros::ok())
  {
    // if(global_flag && (pose_flag == 0)){
    //   setWhichGoal();
    // }
    // else if(pose_flag==2){
    //   global_flag=1;
    // }
    // else{}
  
    ros::spinOnce();
    // rate_loop.sleep();
  }
  return 0; 
}  