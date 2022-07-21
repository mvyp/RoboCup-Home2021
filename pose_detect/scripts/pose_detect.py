#!/usr/bin/env python
import rospy
from tfpose_ros.msg import Persons
from std_msgs.msg import String
flag='0'
a = []
def callback(data):
    global flag
    global a
    pub = rospy.Publisher('follow_flag', String, queue_size=10)
    rate = rospy.Rate(10) # 10hz
    if(data.persons):
        a=data.persons[0]
        coord_list=a.body_part
    else:
        coord_list=[]
    xs=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]
    ys=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]
    for i in range(len(coord_list)):
        current_part=coord_list[i]
        if(current_part.part_id==0):
            xs[0]=current_part.x
            ys[0]=current_part.y
        if(current_part.part_id==1):
            xs[1]=current_part.x
            ys[1]=current_part.y
        if(current_part.part_id==2):
            xs[2]=current_part.x
            ys[2]=current_part.y
        if(current_part.part_id==3):
            xs[3]=current_part.x
            ys[3]=current_part.y
        if(current_part.part_id==4):
            xs[4]=current_part.x
            ys[4]=current_part.y
        if(current_part.part_id==5):
            xs[5]=current_part.x
            ys[5]=current_part.y
        if(current_part.part_id==6):
            xs[6]=current_part.x
            ys[6]=current_part.y
        if(current_part.part_id==7):
            xs[7]=current_part.x
            ys[7]=current_part.y
        if(current_part.part_id==10):
            xs[10]=current_part.x
            ys[10]=current_part.y
        if(current_part.part_id==13):
            xs[13]=current_part.x
            ys[13]=current_part.y
    d1=abs(xs[4]-xs[5])+abs(ys[4]-ys[5])
    # d2=abs(xs[2]-xs[3])+abs(xs[5]-xs[6])+abs(ys[3]-ys[4])+abs(ys[6]-ys[7])
    d2=abs(xs[2]-xs[7])+abs(ys[2]-ys[7])
    if(d1<=0.1):
        flag='1'
    if(d2<=0.15):
        flag='0'
    print(flag)
    pub.publish(flag)
    #print(d2)
    
def listener():
    rospy.init_node('pose_detect', anonymous=True)
    rospy.Subscriber("/pose_estimator/pose", Persons, callback)
    rospy.spin()
 
if __name__ == '__main__':
    listener()
