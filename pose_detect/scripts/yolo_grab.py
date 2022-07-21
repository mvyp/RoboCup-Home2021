#!/usr/bin/env python
import rospy
from darknet_ros_msgs.msg import BoundingBoxes
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Pose2D
from geometry_msgs.msg import Pose
from sensor_msgs.msg import JointState
from std_msgs.msg import String

def GrabActionCallback(pose_msg):
    fObjGrabX = pose_msg.position.x;
    fObjGrabY = pose_msg.position.y;
    fObjGrabZ = pose_msg.position.z;
    ROS_WARN('[OBJ_TO_GRAB] x = ',fObjGrabX,' y = ',fObjGrabY,' z = ',fObjGrabZ)
    ctrl_msg.data = 'pose_diff reset'
    odom_ctrl_pub.publish(ctrl_msg)

    # ajudge the dist to obj
    fMoveTargetX = fObjGrabX - fTargetGrabX 
    fMoveTargetY = fObjGrabY - fTargetGrabY + grab_y_offset 
    ROS_WARN('[MOVE_TARGET] x = ',fMoveTargetX,' y = ', fMoveTargetY)
    nStep = STEP_OBJ_DIST;

def PoseDiffCallback(pose2d_msg): 
    pose_diff.x = pose2d_msg.x;
    pose_diff.y = pose2d_msg.y;
    pose_diff.theta = pose2d_msg.theta;

def VelFixed(inVel,inMax): 
    retVel = inVel;
    if(retVel > inMax):
        retVel = inMax
    if(retVel < -inMax):
        retVel = -inMax
    return retVel

def VelCmd(inVx , inVy, inTz):
    vel_cmd = Twist()
    vel_cmd.linear.x = VelFixed(inVx , vel_max)
    vel_cmd.linear.y = VelFixed(inVy , vel_max)
    vel_cmd.angular.z = VelFixed(inTz , vel_max)
    vel_pub.publish(vel_cmd)

def BehaviorCB(string_msg):
    nFindIndex = string_msg.data.find("grab stop")
    if( nFindIndex >= 0 ):
        ROS_WARN("[grab_stop] ")
        nStep = STEP_WAIT
        vel_cmd=Twist()
        vel_cmd.linear.x = 0
        vel_cmd.linear.y = 0
        vel_cmd.linear.z = 0
        vel_cmd.angular.x = 0
        vel_cmd.angular.y = 0
        vel_cmd.angular.z = 0
        vel_pub.publish(vel_cmd)

def toward_obj(obj_class,obj_list):
    global pub
    rate = rospy.Rate(200)
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

def yolo_callback(data):
    a = data.bounding_boxes
    #toward_obj('water',a)
    #print(a[0])
    
if __name__ == '__main__':
    rospy.init_node('yolo_grab', anonymous=True)
    grab_y_offset = 0.0        
    grab_lift_offset = 0.0     
    grab_forward_offset = 0.0  
    grab_gripper_value = 0.07  
    vel_max = 0.5              

    STEP_WAIT = 0
    STEP_FIND_PLANE = 1
    STEP_PLANE_DIST = 2
    STEP_FIND_OBJ = 3
    STEP_OBJ_DIST = 4
    STEP_HAND_UP = 5
    STEP_FORWARD = 6
    STEP_GRAB = 7
    STEP_OBJ_UP = 8
    STEP_BACKWARD = 9
    STEP_DONE = 10
    nStep = STEP_WAIT

    pc_topic=String()

    vel_pub = rospy.Publisher('/cmd_vel', Twist,queue_size=30)
    mani_ctrl_pub = rospy.Publisher('/wpb_home/mani_ctrl',JointState,queue_size=30)
    mani_ctrl_msg=JointState()
    result_pub = rospy.Publisher('/wpb_home/grab_result',String,queue_size=30)
    result_msg=String()

    rospy.Subscriber("/wpb_home/grab_action", Pose, GrabActionCallback)
    rospy.Subscriber("/wpb_home/behaviors", String, BehaviorCB)

    odom_ctrl_pub = rospy.Publisher('/wpb_home/ctrl',String,queue_size=30)

    rospy.Subscriber("/darknet_ros/pose_diff", Pose2D, PoseDiffCallback)
    rospy.Subscriber("/darknet_ros/bounding_boxes", BoundingBoxes, yolo_callback)

    ctrl_msg = String()
    pose_diff=Pose2D()

    fObjGrabX = 0
    fObjGrabY = 0
    fObjGrabZ = 0
    fMoveTargetX = 0
    fMoveTargetY = 0

    nTimeDelayCounter = 0

    fTargetGrabX = 1.2     
    fTargetGrabY = 0.0     

    mani_ctrl_msg.name = [' ',' ']
    mani_ctrl_msg.position = [0,0]
    mani_ctrl_msg.velocity = [0,0]
    mani_ctrl_msg.name[0] = 'lift'
    mani_ctrl_msg.name[1] = 'gripper'
    mani_ctrl_msg.position[0] = 0
    mani_ctrl_msg.velocity[0] = 0.5 
    mani_ctrl_msg.position[1] = 0.16
    mani_ctrl_msg.velocity[1] = 5   

    rate = rospy.Rate(30) 

    while(1): 
        if(nStep == STEP_OBJ_DIST):
            print('STEP_OBJ_DIST')
            vx = (fMoveTargetX - pose_diff.x)/2
            vy = (fMoveTargetY - pose_diff.y)/2
            VelCmd(vx,vy,0) 

            if(abs(vx) < 0.01 and abs(vy) < 0.01):
                VelCmd(0,0,0)
                ctrl_msg.data = 'pose_diff reset'
                odom_ctrl_pub.publish(ctrl_msg)
                nTimeDelayCounter = 0
                nStep = STEP_HAND_UP
            result_msg.data = 'object x'
            result_pub.publish(result_msg)

        if(nStep == STEP_HAND_UP):
            print('STEP_HAND_UP')
            if(nTimeDelayCounter == 0):
                mani_ctrl_msg.position[0] = fObjGrabZ + grab_lift_offset 
                mani_ctrl_msg.position[1] = 0.16
                mani_ctrl_pub.publish(mani_ctrl_msg)
                result_msg.data = 'hand up'
                result_pub.publish(result_msg)
            nTimeDelayCounter = nTimeDelayCounter + 1
            mani_ctrl_pub.publish(mani_ctrl_msg)
            VelCmd(0,0,0)
            if(nTimeDelayCounter > 20*30):
                fMoveTargetX = fTargetGrabX -0.65 + grab_forward_offset
                fMoveTargetY = 0
                nTimeDelayCounter = 0
                #ctrl_msg.data = "pose_diff reset";
                #odom_ctrl_pub.publish(ctrl_msg);
                nStep = STEP_FORWARD
        if(nStep == STEP_FORWARD):
            print('STEP_FORWARD')
            vx = (fMoveTargetX - pose_diff.x)/2 
            vy = (fMoveTargetY - pose_diff.y)/2 
            VelCmd(vx,vy,0)
            if(abs(vx) < 0.01 and abs(vy) < 0.01):
                VelCmd(0,0,0) 
                ctrl_msg.data = 'pose_diff reset'
                odom_ctrl_pub.publish(ctrl_msg)
                nTimeDelayCounter = 0
                nStep = STEP_GRAB

            result_msg.data = 'forward'
            result_pub.publish(result_msg)

        if(nStep == STEP_GRAB):
            print('STEP_GRAB')
            if(nTimeDelayCounter == 0):
                result_msg.data = 'grab'
                result_pub.publish(result_msg)
            mani_ctrl_msg.position[1] = grab_gripper_value  
            mani_ctrl_pub.publish(mani_ctrl_msg)
            nTimeDelayCounter = nTimeDelayCounter + 1
            VelCmd(0,0,0)
            if(nTimeDelayCounter > 8*30):
                nTimeDelayCounter = 0
                nStep = STEP_OBJ_UP 

        if(nStep == STEP_OBJ_UP):
            print('STEP_OBJ_UP')
            if(nTimeDelayCounter == 0):
                mani_ctrl_msg.position[0] = mani_ctrl_msg.position[0] + 0.03
                mani_ctrl_pub.publish(mani_ctrl_msg)
                result_msg.data = 'object up'
                result_pub.publish(result_msg)
            nTimeDelayCounter = nTimeDelayCounter + 1
            VelCmd(0,0,0)
            mani_ctrl_pub.publish(mani_ctrl_msg)
            if(nTimeDelayCounter > 3*30):
                fMoveTargetX = -(fObjGrabX -0.65 + grab_forward_offset)
                fMoveTargetY = -(fObjGrabY + grab_y_offset) 
                nTimeDelayCounter = 0
                nStep = STEP_BACKWARD 

        if(nStep == STEP_BACKWARD):
            print('STEP_BACKWARD')
            vx = (fMoveTargetX - pose_diff.x)/2
            if(pose_diff.x > -0.05):
                vy = 0
            else:
                vy = (fMoveTargetY - pose_diff.y)/2
            VelCmd(vx,vy,0)

            if(abs(vx) < 0.01 and abs(vy) < 0.01): 
                VelCmd(0,0,0)
                ctrl_msg.data = 'pose_diff reset'
                odom_ctrl_pub.publish(ctrl_msg)
                nTimeDelayCounter = 0
                nStep = STEP_DONE
            result_msg.data = 'backward'
            result_pub.publish(result_msg)

        if(nStep == STEP_DONE):
            print('STEP_DONE')
            if(nTimeDelayCounter < 10):
                VelCmd(0,0,0)
                nTimeDelayCounter = nTimeDelayCounter + 1
                result_msg.data = 'done'
                result_pub.publish(result_msg)
            else:
                nStep = STEP_WAIT
        rospy.spin()
        rate.sleep()
