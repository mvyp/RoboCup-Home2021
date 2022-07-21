#include <ros/ros.h> 
#include <move_base_msgs/MoveBaseAction.h> 
#include <actionlib/client/simple_action_client.h> 
#include "std_msgs/String.h"

using namespace std;
bool Stop_flag = 0;
ros::Publisher cancel_pub_;

void domsg2(const std_msgs::String::ConstPtr &msg2)
{
    actionlib_msgs::GoalID stop_goal;
    ROS_INFO("Got follow_flag = %s",msg2->data.c_str());
    if((msg2->data=="1")||(msg2->data=="2"))
        Stop_flag = 1;
    else if(msg2->data=="0")
        Stop_flag = 0;
}
int main(int argc, char** argv)
{ 
    ros::init(argc, argv, "move_pause"); 
    ros::NodeHandle nh;
    ros::Publisher cancel_pub_ =  nh.advertise<actionlib_msgs::GoalID>("move_base/cancel",1);
    ros::Subscriber flag_sub2 = nh.subscribe("follow_flag",10,domsg2);
    actionlib_msgs::GoalID stop_goal;
    ros::Rate rate_loop(10);
    while(ros::ok())
    {
        if(Stop_flag)
            cancel_pub_.publish(stop_goal);
        else{}
        // ROS_WARN("setgoal unexpected ERROR!");
    
        ros::spinOnce();
        rate_loop.sleep();
    }



    return 0; 
}  