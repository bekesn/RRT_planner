#ifndef MAPHANDLER_H
#define MAPHANDLER_H

#include "ros/ros.h"
#include "SearchTree.h"
#include "VehicleModel.h"
#include "frt_custom_msgs/Map.h"
#include "frt_custom_msgs/Landmark.h"

class MapHandler
{
private:
    
    std::vector<frt_custom_msgs::Landmark*> map;
    VehicleModel* vehicleModel;
    SS_VECTOR goalState;
    bool mapReceived;

    // Parameter struct
    RRT_PARAMETERS* param;

public:
    MapHandler();
    MapHandler(VehicleModel* vehicleModel);
    //~MapHandler();
    
    // Check for offcourse
    bool isOffCourse(PATH_TYPE* path);

    // Check for collision with lines
    bool isOnTrackEdge(SS_VECTOR* vehicleState, std::vector<frt_custom_msgs::Landmark*>* cones);

    // Get random state
    SS_VECTOR getRandomState(PATH_TYPE* path);

    // Calculate and get goal state
    void calculateGoalState();
    SS_VECTOR getGoalState();

    // Update map
    void mapCallback(const frt_custom_msgs::Map::ConstPtr &msg);

    // Visualize significant points
    void visualizePoints(visualization_msgs::MarkerArray* mArray);

    // Get closest cone by color
    frt_custom_msgs::Landmark* getClosestLandmark(frt_custom_msgs::Landmark* landmark, frt_custom_msgs::Landmark::_color_type color);

    // Return if map arrived
    bool hasMap();

};



#endif //MAPHANDLER_H