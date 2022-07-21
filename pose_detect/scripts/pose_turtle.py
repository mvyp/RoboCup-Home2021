#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Twist
 
def callback(data):
    pub = rospy.Publisher('/turtle1/cmd_vel', Twist,queue_size=10)
    rate = rospy.Rate(10) # 10hz
    msg = Twist() 
    if(data.data=='1'):
        msg.linear.x=5
        msg.angular.z=5
    else:
        msg.linear.x=0
        msg.angular.z=0
    pub.publish(msg)
    rate.sleep()
def listener():
    rospy.init_node('pose_turtle', anonymous=True)
    rospy.Subscriber("follow_flag", String, callback)
    rospy.spin()
 
if __name__ == '__main__':
    listener()
