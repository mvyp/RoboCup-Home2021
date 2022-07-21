#include <ros/ros.h> 
#include <move_base_msgs/MoveBaseAction.h> 
#include <actionlib/client/simple_action_client.h> 
#include "geometry_msgs/PoseWithCovarianceStamped.h"    
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
 
using namespace std;
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; 

bool gotoSofa=false;
int emptySeat=0;
typedef struct _POSE
{
  double X;
  double Y;
  double cita;
} POSE;
POSE home= {6.037,-2.615,1.509};
POSE door={};
POSE sofa= {0.667,1.053,-3.057};
POSE sofa1= {0.667,1.053,-3.057};
POSE sofa2= {0.667,1.053, 2.077};
POSE sofa3= {0.667,1.053, -2.677};

void setHome( ros::Publisher pub)
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
  else 
    ROS_ERROR("Failed to move to the goal!");  
}
 
void SofaCallback(const std_msgs::Bool::ConstPtr &msg)
{
  // std::cout<<"get go_to_sofa: " << msg->data << std::endl;
  if(msg->data){
    gotoSofa = true;
  }
}  

void SeatCallback(const std_msgs::Bool::ConstPtr &msg)
{
  // std::cout<<"get empty_seat:" << msg->data << std::endl;
  if(msg->data==1)
    emptySeat=1;
  else if (msg->data==2)
    emptySeat=2;
  else if (msg->data==3)
    emptySeat=3;
}  

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "receptionost_nav"); 
  ros::NodeHandle nh;
  ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);
  ros::Subscriber goto_sofa = nh.subscribe("/go_to_sofa",1,SofaCallback); 
  ros::Subscriber arrange_seat = nh.subscribe("/empty_seat",8,SeatCallback); 
  ros::Publisher finish_dool = nh.advertise<std_msgs::String>("/finish_dool_nav", 10);
  ros::Publisher finish_sofa = nh.advertise<std_msgs::String>("/finish_sofa_nav", 10);
  ros::Publisher finish_seat = nh.advertise<std_msgs::String>("/finish_sofa_nav", 10);
  setHome(pub_initialpose);
  setGoal(door);
  while(ros::ok())
  {
    if(gotoSofa){
      setGoal(sofa);
      break;
    } 
    if(emptySeat==1)
      setGoal(sofa1);
    else if(emptySeat==2)
      setGoal(sofa2);
    else if(emptySeat==3)
      setGoal(sofa3);
    

  }
  return 0; 
}  
