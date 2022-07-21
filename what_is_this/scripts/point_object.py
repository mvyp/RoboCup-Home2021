#!/usr/bin/env python
#!coding=utf-8

import rospy
import math
from darknet_ros_msgs.msg import BoundingBoxes
from tfpose_ros.msg import Persons
from std_msgs.msg import String

flag = '0'
a = []
name=''
xmin=0
xmax=0
ymin=0
ymax=0
jiaodu1=0
jiaodu2=1
jiaodu3=2
jiaodu4=3
q=[]
def darknet_callback(data):
    global q
    q = data.bounding_boxes


def pose_callback(data):
    global flag,a,q,jiaodu1,jiaodu2,jiaodu3,jiaodu4,m,n,j,k,xs,ys
    for i in q:
        name= i.Class
        xmin = i.xmin
        xmax=i.xmax
        ymin=i.ymin
        ymax=i.ymax
        #print(name)
        if (data.persons):
            a = data.persons[0]
            coord_list = a.body_part
        else:
            coord_list = []
        xs = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
        ys = [0, 1, 2, 3, 4, 5, 6, 70, 8, 9, 10, 11, 12, 13, 14]
        for i in range(len(coord_list)):
            current_part = coord_list[i]
            if (current_part.part_id == 2):
                xs[2] = current_part.x
                ys[2] = current_part.y
            if (current_part.part_id == 3):
                xs[3] = current_part.x
                ys[3] = current_part.y
            if (current_part.part_id == 4):
                xs[4] = current_part.x
                ys[4] = current_part.y
            if (current_part.part_id == 5):
                xs[5] = current_part.x
                ys[5] = current_part.y
            if (current_part.part_id == 6):
                xs[6] = current_part.x
                ys[6] = current_part.y
            if (current_part.part_id == 7):
                xs[7] = current_part.x
                ys[7] = current_part.y
    
        x=(xmin+xmax)/2
        y=(ymin+ymax)/2

        xs[3] = xs[3]*1920
        xs[4] = xs[4]*1920 #人的右手
        xs[6] = xs[6]*1920
        xs[7] = xs[7]*1920 #人的左手

        ys[3] = ys[3]*1080
        ys[4] = ys[4]*1080
        ys[6] = ys[6]*1080
        ys[7] = ys[7]*1080

        try:
            d1=xs[4]-xs[3]
            d2=ys[4]-ys[3]
            d3=xs[7]-xs[6]
            d4=ys[7]-ys[6]
    
            d5=x-xs[4]
            d6=y-ys[4]
            d7=x-xs[7]
            d8=y-ys[7]
    
            m=d2/d1
            n=d4/d3
    
            j=d6/d5
            k=d8/d7

            #jiaodu1=math.atan(m)          #右手臂的角度
            #jiaodu2=math.atan(n)          #左手臂的角度
            #jiaodu3=math.atan(j)          #右手指到质心的角度
            #jiaodu4=math.atan(k)          #左手指到质心的角度
        except:
            pass
        #print(name,x,y)
        #print('lefthand',xs[7],ys[7])
        #if(name=='bowl'):
            #print(name,n,k)
        if(abs(n-k)<=0.3):
            if(name!='human'):
                print(name)
                class_pub = rospy.Publisher('class_flag', String, queue_size=10)
                rate = rospy.Rate(10)  # 10hz
                class_pub.publish(name)
    
def listener():
    rospy.init_node('what_is_that_arm', anonymous=True)
    rospy.Subscriber("/darknet_ros/bounding_boxes", BoundingBoxes, darknet_callback)
    rospy.Subscriber("/pose_estimator/pose", Persons, pose_callback)
    rospy.spin()


if __name__ == '__main__':
    listener()
