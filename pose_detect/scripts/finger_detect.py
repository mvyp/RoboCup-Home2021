#!/usr/bin/env python
import rospy
from tfpose_ros.msg import Persons
from std_msgs.msg import String

flag = '0'
a = []

def callback(data):
    global flag
    global a
    pub = rospy.Publisher('finger_flag', String, queue_size=10)
    rate = rospy.Rate(10)  # 10hz
    if (data.persons):
        a = data.persons[0]
        coord_list = a.body_part
    else:
        coord_list = []
    xs = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
    ys = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
    for i in range(len(coord_list)):
        current_part = coord_list[i]
        if (current_part.part_id == 0):
            xs[0] = current_part.x
            ys[0] = current_part.y
        if (current_part.part_id == 1):
            xs[1] = current_part.x
            ys[1] = current_part.y
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
        if (current_part.part_id == 10):
            xs[10] = current_part.x
            ys[10] = current_part.y
        if (current_part.part_id == 13):
            xs[13] = current_part.x
            ys[13] = current_part.y
    d3=abs(xs[4]-xs[2])
    d4=abs(ys[4]-ys[2])
    d5=d3*d3+d4*d4
    d6=(d5)**0.5

    d7=abs(xs[5]-xs[7])
    d8=abs(ys[5]-ys[7])
    d9=d7*d7+d8*d8
    d10=(d9)**0.5

    if((d3/d6)>=0.34 and (d7/d10)<0.34):
        flag='1'

    if ((d7 / d10) >= 0.34 and (d3 / d6) < 0.34):
        flag='2'
    
   # if ((d3/d6)<0.34 and (d7/d10)<0.34):
       # flag='0'

    print(flag)
    pub.publish(flag)

def listener():
    rospy.init_node('finger_detect', anonymous=True)
    rospy.Subscriber("/pose_estimator/pose", Persons, callback)
    rospy.spin()

if __name__ == '__main__':
    listener()
