//
// Created by sie-rc on 5/9/21.
//

#include "detect_ctl_head.h"
 
int main(int argc, char **argv){
    ros::init(argc,argv,"boxes_ctl_grab");
    YOLO_GRAB yolo_grab;
    ros::spin();
 
    return 0;
}
