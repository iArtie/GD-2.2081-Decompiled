#include "Utils/NodePoint.h"

NodePoint* NodePoint::create(cocos2d::CCPoint point)
{
    NodePoint* returnVal = new NodePoint();
    if (returnVal->init(point))
    {
        returnVal->autorelease();
    }
    else
    {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool NodePoint::init(cocos2d::CCPoint point)
{
    m_point = point;
    return true;
}

NodePoint::~NodePoint()
{
}