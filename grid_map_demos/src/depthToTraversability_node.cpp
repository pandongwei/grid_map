//
// Created by pan on 27.07.20.
//
#include "grid_map_demos/FiltersDemo.hpp"
#include "grid_map_demos/ImageToGridmapDemo.hpp"
#include "grid_map_demos/DepthToTraversability.hpp"
#include <ros/ros.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{

    ros::init(argc, argv, "depth_to_traversability");
    ros::NodeHandle nodeHandle("~");
    bool success = true;
    grid_map_demos::DepthToTraversability depthToTraversability(nodeHandle, success);
    ros::spin();
    return 0;
}