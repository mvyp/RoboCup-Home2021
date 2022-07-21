#include <ros/ros.h> 
#include <move_base_msgs/MoveBaseAction.h> 
#include <actionlib/client/simple_action_client.h> 
#include "geometry_msgs/PoseWithCovarianceStamped.h"    
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <string.h>
 
using namespace std;
typedef actionlib::SimpleActionClient <move_base_msgs::MoveBaseAction> MoveBaseClient; 

bool gotoSofa = false;
int emptySeat = 0;
bool backdoor = false;

typedef struct _POSE
{
  double X;
  double Y;
  double cita;
} POSE;
POSE home= {0,0,0};
POSE door= {1,0,3.14};
POSE sofa= {2.079,0.121,1.570};
POSE sofa1= {2.079,0.121,1.919};
POSE sofa2= {2.079,0.121,1.570};
POSE sofa3= {2.079,0.121,1.220};

void setHome( ros::Publisher pub)
{
  geometry_msgs::PoseWithCovarianceStamped msg_poseinit;
  msg_poseinit.header.frame_id = "map";
  msg_poseinit.header.stamp = ros::Time::now();
  msg_poseinit.pose.pose.position.x =  0;
  msg_poseinit.pose.pose.position.y = 0 ;
  msg_poseinit.pose.pose.orientation.w=1;
  msg_poseinit.pose.pose.orientation.x=0.000;
  msg_poseinit.pose.pose.orientation.y=0.000;
  msg_poseinit.pose.pose.orientation.z=0;
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
 
void SofaCallback(const std_msgs::String::ConstPtr &msg)
{
  // std::cout<<"get go_to_sofa: " << msg->data << std::endl;
  if(msg->data == "guest_go_sofa"){
    gotoSofa = true;
  }
}  

void SeatCallback(const std_msgs::String::ConstPtr &msg)
{
  // std::cout<<"get empty_seat:" << msg->data << std::endl;
  if(msg->data == "0")
    emptySeat = 1;
  else if (msg->data == "1")
    emptySeat = 2;
  else if (msg->data == "2")
    emptySeat = 3;
}  


void backCallback(const std_msgs::String::ConstPtr &msg)
{
  if(msg->data == "back_door")
      backdoor = true;
}

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "receptionist_nav");
  ros::NodeHandle nh;
  ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/initialpose", 10);
  ros::Publisher door_in = nh.advertise<std_msgs::String>("/into_room", 10);
  ros::Publisher sofa_arrive1 = nh.advertise<std_msgs::String>("/guest_one_sofa", 10);
  ros::Publisher sofa_arrive2 = nh.advertise<std_msgs::String>("/guest_two_sofa", 10);
  ros::Publisher seat_success = nh.advertise<std_msgs::String>("/seat_success", 10);
  ros::Publisher get_door = nh.advertise<std_msgs::String>("/get_door", 10);
  std_msgs::String finish_msg;
  std::stringstream ss;

  ros::Subscriber goto_sofa = nh.subscribe("/go_to_sofa",10, SofaCallback);
  ros::Subscriber arrange_seat = nh.subscribe("/empty_seat",8, SeatCallback);
  ros::Subscriber back_door = nh.subscribe("/back_door",10, backCallback);

  setHome(pub_initialpose);

  //第一个客人

    //到门口
    setGoal(door);
    setGoal(sofa);
    // setGoal(sofa1);
    // setGoal(sofa3);
  return 0; 
}  
