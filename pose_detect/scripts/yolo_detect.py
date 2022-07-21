#!/usr/bin/env python
import rospy
from darknet_ros_msgs.msg import BoundingBoxes
from geometry_msgs.msg import Twist


def toward_obj(obj_class,obj_list):
    global pub
    rate = rospy.Rate(200) # 10hz
    obj_in=0
    z=0
    msg = Twist() 
    for i in obj_list:
        if(i.Class==obj_class):
            obj_in=1
            x=960/2-(i.xmin+i.xmax)/2
            z=obj_in*x*0.002
    msg.angular.z=z
    pub.publish(msg)
    print(z)
    rate.sleep()

def callback(data):
    a = data.bounding_boxes
    toward_obj('water',a)
    #print(a[0])
    
def listener():
    rospy.init_node('yolo_detect', anonymous=True)
    rospy.Subscriber("/darknet_ros/bounding_boxes", BoundingBoxes, callback)
    rospy.spin()
 
if __name__ == '__main__':
    pub = rospy.Publisher('/cmd_vel', Twist,queue_size=10)
    
    listener()
