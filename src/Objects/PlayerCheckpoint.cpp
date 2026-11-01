#include "Objects/PlayerCheckpoint.h"

PlayerCheckpoint* PlayerCheckpoint::create()
{
    PlayerCheckpoint* returnVal = new PlayerCheckpoint();
    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool PlayerCheckpoint::init()
{
    if (!cocos2d::CCNode::init())
        return false;

    m_touchingRings = cocos2d::CCArray::create();
    m_touchingRings->retain();

    return true;
}

PlayerCheckpoint::~PlayerCheckpoint()
{
}