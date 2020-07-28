//
// Created by pan on 27.07.20.
//


#include <grid_map_cv/GridMapCvConverter.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "grid_map_demos/DepthToTraversability.hpp"
#include <iostream>
using namespace cv;
using namespace grid_map;
using namespace std;

namespace grid_map_demos {

    DepthToTraversability::DepthToTraversability(ros::NodeHandle& nodeHandle, bool& success)
            : nodeHandle_(nodeHandle),
              filterChain_("grid_map::GridMap"),
              map_(grid_map::GridMap({"elevation"})),
              mapInitialized_(false)
    {

        if (!readParameters()) {
            success = false;
            return;
        }
        map_.setBasicLayers({"elevation"});
        imageSubscriber_ = nodeHandle_.subscribe(imageTopic_, 1000, &DepthToTraversability::callback, this);
//        imagePublisher_ = nodeHandle_.advertise<grid_map_msgs::GridMap>(outputTopic_, 1, true);

        // Setup filter chain.
        if (!filterChain_.configure(filterChainParametersName_, nodeHandle)) {
            ROS_ERROR("Could not configure the filter chain!");
            success = false;
            return;
        }

        success = true;
    }

    DepthToTraversability::~DepthToTraversability()
    {
    }

    bool DepthToTraversability::readParameters()
    {

        nodeHandle_.param("image_topic", imageTopic_, std::string("/image_publisher/image"));
        nodeHandle_.param("resolution", resolution_, 0.03);
        nodeHandle_.param("min_height", minHeight_, 0.0);
        nodeHandle_.param("max_height", maxHeight_, 1.0);

//        nodeHandle_.param("output_topic", outputTopic_, std::string("output"));
        nodeHandle_.param("filter_chain_parameter_name", filterChainParametersName_, std::string("grid_map_filters"));
        return true;
    }

    void DepthToTraversability::callback(const sensor_msgs::Image& msg)
    {

        std::string save_path = msg.header.frame_id;
        // image to grid map
        if (!mapInitialized_) {
            grid_map::GridMapRosConverter::initializeFromImage(msg, resolution_, map_);
            ROS_INFO("Initialized map with size %f x %f m (%i x %i cells).", map_.getLength().x(),
                     map_.getLength().y(), map_.getSize()(0), map_.getSize()(1));
            mapInitialized_ = true;
        }
        grid_map::GridMapRosConverter::addLayerFromImage(msg, "elevation", map_, minHeight_, maxHeight_);
        grid_map::GridMapRosConverter::addColorLayerFromImage(msg, "color", map_);

        // Apply filter chain.
        grid_map::GridMap outputMap;
        if (!filterChain_.update(map_, outputMap)) {
            ROS_ERROR("Could not update the grid map filter chain!");
            return;
        }

        // TODO
        cv::Mat outputImg;
        GridMapCvConverter::toImage<unsigned short, 1>(outputMap, "traversability",CV_16UC1,outputImg);
        cv::imwrite(save_path, outputImg);


    }

} /* namespace */