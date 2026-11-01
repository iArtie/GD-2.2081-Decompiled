#include "PointNode.h"

PointNode* PointNode::create(cocos2d::CCPoint point)
{
    PointNode* returnVal = new PointNode();

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

bool PointNode::init(cocos2d::CCPoint point)
{
    if (!cocos2d::CCObject::init())
        return false;

    m_point = point;
    return true;
}

PointNode::~PointNode()
{
}