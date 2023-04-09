#include "VehicleModel.h"

VehicleModel::VehicleModel()
{
    
}

VehicleModel::VehicleModel(VEHICLE_PARAMETERS* par)
{
    vehicleParam = par;
}


void VehicleModel::poseCallback(const geometry_msgs::PoseStamped::ConstPtr &msg)
{
    tf::Quaternion q(msg->pose.orientation.x, msg->pose.orientation.y, msg->pose.orientation.z, msg->pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);


    currentPose.x = msg->pose.position.x;
    currentPose.y = msg->pose.position.y;
    currentPose.theta = yaw;

}

geometry_msgs::Pose2D VehicleModel::getCurrentPose()
{
    return currentPose;
}


VEHICLE_PARAMETERS* VehicleModel::getParameters()
{
    return vehicleParam;
}

PATH_TYPE* VehicleModel::simulateToTarget(SS_VECTOR startState, SS_VECTOR goalState, RRT_PARAMETERS* param)
{
    PATH_TYPE* trajectory;

    switch(vehicleParam->simType)
    {
        case HOLONOMIC:
            trajectory = simulateHolonomic(startState, goalState, param);
            break;
        case HOLONOMIC_CONSTRAINED:
            trajectory = simulateHolonomicConstrained(startState, goalState, param);
            break;
        case BICYCLE_SIMPLE:
            //TODO
            break;
        case BICYCLE:
            //TODO
            break;
        default:
            throw std::invalid_argument("Wrong simulation type");
            break;
    }

    return trajectory;
}

double VehicleModel::distance(SS_VECTOR start, SS_VECTOR goal)
{
    double distance;
    switch(vehicleParam->distType)
    {
        case EUCLIDEAN:
            distance = getDistEuclidean(start, goal);
            break;
        case SIMULATED:
            //TODO
            break;
        default:
            throw std::invalid_argument("Wrong distance calculation type");
            break;
    }

    return distance;
}

double VehicleModel::cost(PATH_TYPE* trajectory)
{
    double cost;
    switch(vehicleParam->costType)
    {
        case EUCLIDEAN:
            cost = getDistanceCost(trajectory);
            break;
        case SIMULATED:
            cost = getTimeCost(trajectory);
            break;
        default:
            throw std::invalid_argument("Wrong cost calculation type");
            break;
    }

    return cost;
}

PATH_TYPE* VehicleModel::simulateHolonomic(SS_VECTOR start, SS_VECTOR goal, RRT_PARAMETERS* param)
{
    float distance, ratio, x, y;
    int i, numOfStates;
    Eigen::Vector3d startVec(start.data());
    Eigen::Vector3d goalVec(goal.data());
    PATH_TYPE* path = new PATH_TYPE;

    float maxConndist = param->maxVelocity * param->simulationTimeStep;
    distance = getDistEuclidean(start, goal);
    ratio = maxConndist / distance;
    if (ratio > 1) ratio = 1;
    numOfStates = (int) (ratio*distance/param->resolution);

    for(i = 0; i < numOfStates; i++)
    {
        x = start[0] + (goal[0] - start[0]) * ratio * (i+1) / (float) numOfStates;
        y = start[1] + (goal[1] - start[1]) * ratio * (i+1) / (float) numOfStates;
        path->push_back({x, y, goal[2]});
    }
    return path;
}

PATH_TYPE* VehicleModel::simulateHolonomicConstrained(SS_VECTOR start, SS_VECTOR goal, RRT_PARAMETERS* param, float maxAngle)
{

    float distance, ratio, x, y, orientation, dx, dy;
    int i, numOfStates;
    Eigen::Vector3d startVec(start.data());
    Eigen::Vector3d goalVec(goal.data());
    PATH_TYPE* path = new PATH_TYPE;

    float maxConndist = param->maxVelocity * param->simulationTimeStep;
    distance = getDistEuclidean(start, goal);
    float angleDiff = angularDifference(start, goal);
    if ((-maxAngle <= angleDiff ) && (angleDiff < maxAngle))
    {
        orientation = atan2((goal[1] - start[1]), (goal[0] - start[0]));
        
    }
    else if ((-M_PI <= angleDiff ) && (angleDiff < -maxAngle))
    {
        orientation = std::remainder(start[2] - maxAngle, 2.0*M_PI);
    }
    else
    {
        orientation = std::remainder(start[2] + maxAngle, 2.0*M_PI);
    }


    dx = cos(orientation) * distance;
    dy = sin(orientation) * distance;

    ratio = maxConndist / distance;
    if (ratio > 1) ratio = 1;
    numOfStates = (int) (ratio*distance/param->resolution);

    for(i = 0; i < numOfStates; i++)
    {
        x = start[0] + dx * ratio * (i+1) / (float) numOfStates;
        y = start[1] + dy * ratio * (i+1) / (float) numOfStates;
        path->push_back({x, y, orientation});
    }
    return path;
}

double VehicleModel::getDistEuclidean(SS_VECTOR start, SS_VECTOR goal)
{
    double dx = start[0]- goal[0];
    double dy = start[1] - goal[1];
    return(sqrt(dx*dx + dy*dy));
}

double VehicleModel::getDistanceCost(PATH_TYPE* trajectory)
{
    if(trajectory->size() < 2) return 0;
    
    SS_VECTOR prevState = (*trajectory)[0];
    SS_VECTOR currState;
    int size = trajectory->size();
    double length = 0;
    for (int i = 1; i < size; i++)
    {
        currState = (*trajectory)[i];
        length += getDistEuclidean(currState, prevState);
        prevState = currState;
    }
    return length;
}

double VehicleModel::getTimeCost(PATH_TYPE* trajectory)
{
    double elapsed = 0;
    //TODO
    return elapsed;
}

double VehicleModel::angularDifference(SS_VECTOR vehicleState, SS_VECTOR target)
{
    auto dy = target[1] - vehicleState[1];
    auto dx = target[0] - vehicleState[0];
    auto angle = atan2(dy, dx);
    auto diff = angle - vehicleState[2];
    auto res = std::remainder(diff, 2*M_PI);
    //ROS_INFO_STREAM("" << dy << "  " << dx << "  "  << angle << "  "  << diff << "  "  << res);
    return res;
}
    