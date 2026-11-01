#pragma once

#include "Platform/cocos2d.h"

class PointNode : public cocos2d::CCObject {
public:
    static PointNode* create(cocos2d::CCPoint point);

    bool init(cocos2d::CCPoint point);

    virtual ~PointNode();

protected:
    cocos2d::CCPoint m_point;
};