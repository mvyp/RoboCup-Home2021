#include <ros/ros.h> 
#include <move_base_msgs/MoveBaseAction.h> 
#include <actionlib/client/simple_action_client.h> 
#include "geometry_msgs/PoseWithCovarianceStamped.h"    
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include "std_msgs/Int32.h"
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <string.h>
#include <sensor_msgs/JointState.h>
 
using namespace std;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; 
ros::Publisher finish_goal;
ros::Publisher intro;
ros::Publisher finish_seat;
ros::Publisher finish_door;
ros::Publisher voice_pub;
ros::Publisher sofa_view;
// ros::Publisher words_pub;

std_msgs::String finish_msg;
// std_msgs::String words_msg;
std::stringstream ss;
std::stringstream ss7;

std_msgs::String intro_msg;
std::stringstream ss1;

std_msgs::String seat_msg;
std::stringstream ss2;

std_msgs::String door_msg;
std::stringstream ss3;

std::stringstream ss4;
std::stringstream ss5;

std_msgs::String view_msg;
std::stringstream ss6;

std::stringstream ss9;

// 机械臂部分
sensor_msgs::JointState mani_ctrl_msg;
ros::Publisher mani_ctrl_pub;
int nTimeDelayCounter = 0;
int intro_count=1;
int hostseat;
int emptyseat;

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
  std::cout<<"get go_to_sofa: " << msg->data << std::endl;
  if(msg->data=="gotoroom"){
    setGoal(sofa);
    ROS_INFO("already goto room");
  }else if(msg->data=="gotoroom2"){
    setGoal(sofa);
  }
  std::stringstream ss6;
  sleep(3);
  ss6 << "point_empty2";
  view_msg.data=ss6.str();
  sofa_view.publish(view_msg);
}  


void SeatCallback(const std_msgs::Int32::ConstPtr &msg)
{ 
  ROS_INFO("%d", msg->data);
  if(msg->data==0 || msg->data==1 || msg->data==2 || msg->data==3 ){
    if(msg->data==0){
      setGoal(sofa1);
    }
    else if(msg->data==1){
      setGoal(sofa2);
    }
    else if(msg->data==2){
      setGoal(sofa3);
    }
    else if (msg->data==3)
      setGoal(sofa4);
    } 
    std::stringstream ss2;
    ss2 << "arm_go";
    seat_msg.data = ss2.str();
    finish_seat.publish(seat_msg);
}

void hostCallback(const std_msgs::Int32::ConstPtr &msg){
  hostseat=msg->data;
}

void armCallback(const std_msgs::String::ConstPtr &msg){
  if(msg->data == "arm_done"){
    if(hostseat==0)
      setGoal(sofa1);
    else if(hostseat==1)
      setGoal(sofa2);
    else if(hostseat==2)
      setGoal(sofa3);
    else if(hostseat==3)
      setGoal(sofa4);
    
    ss9 << "second1";
    intro_msg.data = ss7.str();
    intro.publish(intro_msg);
  }
}

void backCallback(const std_msgs::String::ConstPtr &msg)
{
  if(msg->data=="1"){
    setGoal(door);
    ss5 << "Please come in.";
    std_msgs::String voice_msg;
    voice_msg.data = ss5.str();
    voice_pub.publish(voice_msg);
    sleep(2);
    ss3 << "finish_door2";
    door_msg.data = ss3.str();
    finish_goal.publish(door_msg);
  }
}

void turnCallback(const std_msgs::String::ConstPtr &msg){
  if(msg->data=="1"){
    if(emptyseat==0)
      setGoal(sofa1);
    else if (emptyseat==1)
      setGoal(sofa2);
    else if(emptyseat==2)
      setGoal(sofa3);
    else if(emptyseat==3)
      setGoal(sofa4);
    ss1 << "second2";
    intro_msg.data = ss1.str();
    intro.publish(intro_msg);
  }
}

void backsofaCallback(const std_msgs::String::ConstPtr &msg){
  if(msg->data=="gotoroom2"){
    setGoal(sofa);
    ROS_INFO("already goto room2");
  }
    sleep(3);
  ss6 << "point_empty";
  view_msg.data=ss6.str();
  sofa_view.publish(view_msg);
}

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "receptionist_nav_robocup"); 
  ros::NodeHandle nh;
  ros::Rate loop_rate(30);
  ros::Publisher pub_initialpose = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 10);
  finish_goal = nh.advertise<std_msgs::String>("finish", 1000);
  intro = nh.advertise<std_msgs::String>("introduce", 1000); // point_empty
  finish_seat = nh.advertise<std_msgs::String>("finish_seat", 1000);
  voice_pub = nh.advertise<std_msgs::String>("voiceWords1", 10);
  // words_pub = nh.advertise<std_msgs::String>("voiceWords1", 10);
  sofa_view= nh.advertise<std_msgs::String>("sofa_empty",10);

  ros::Subscriber goto_sofa = nh.subscribe("gotoroom",1000,SofaCallback); 
  //ros::Subscriber goto_sofa2 = nh.subscribe("gotoroom2",1000,SofaCallback2); 
  ros::Subscriber arrange_seat = nh.subscribe("empty_seat",10,SeatCallback);
  ros::Subscriber host_seat = nh.subscribe("sit_seat",10,hostCallback); 
  ros::Subscriber arm_finish = nh.subscribe("arm_finish",10,armCallback);//机械壁发我
  ros::Subscriber back_door = nh.subscribe("backdoor",10,backCallback); 

  ros::Subscriber turn_first= nh.subscribe("turn_empty",10,turnCallback); 
  ros::Subscriber back_sofa=nh.subscribe("",10,backsofaCallback);

  setHome(pub_initialpose);

  setGoal(door);
  ss << "finish_door";
  finish_msg.data =ss.str();
  finish_goal.publish(finish_msg);
  ROS_INFO("already send");


  ros::spin();

  return 0; 
}
