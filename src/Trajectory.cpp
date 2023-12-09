#include "Trajectory.h"


template<class StateSpaceVector>
Trajectory<StateSpaceVector>::Trajectory(/* args */)
{
}

template<class StateSpaceVector>
Trajectory<StateSpaceVector>::~Trajectory()
{
}

template<class StateSpaceVector>
double Trajectory<StateSpaceVector>::cost(const unique_ptr<RRT_PARAMETERS>& param) const
{
    double cost;
    switch(param->costType)
    {
        case DISTANCE:
            cost = this->getDistanceCost();
            break;
        case TIME:
            cost = this->getTimeCost();
            break;
        default:
            throw std::invalid_argument("Wrong cost calculation type");
            break;
    }

    return cost;
}

template<class StateSpaceVector>
double Trajectory<StateSpaceVector>::getDistanceCost(void) const
{
    if(this->size() < 2) return 100;
    
    shared_ptr<StateSpace2D> prevState = (*this)[0];
    shared_ptr<StateSpace2D> currState;
    int size = this->size();
    double length = 0;
    for (int i = 1; i < size; i++)
    {
        currState = (*this)[i];
        length += prevState->getDistEuclidean(*currState);
        prevState = currState;
    }
    return length;
}

template<class StateSpaceVector>
double Trajectory<StateSpaceVector>::getTimeCost(void) const
{
    if(this->size() < 2) return 100;
    
    shared_ptr<StateSpace2D> prevState = (*this)[0];
    shared_ptr<StateSpace2D> currState;
    int size = this->size();
    double elapsed = 0;
    for (int i = 1; i < size; i++)
    {
        currState = (*this)[i];
        if(prevState->vx() > 0)
        {
            elapsed += prevState->getDistEuclidean(*currState) / prevState->vx();
        }
        else
        {
            elapsed += 100;
        }
        prevState = currState;
    }
    return elapsed;
}

// Define classes
template class Trajectory<StateSpace2D>;
template class Trajectory<KinematicBicycle>;
template class Trajectory<DynamicBicycle>;