#pragma once

#include "Platform/cocos2d.h"

class NodePoint : public cocos2d::CCObject {
public:
    static NodePoint* create(cocos2d::CCPoint point);

    bool init(cocos2d::CCPoint point);

    virtual ~NodePoint();

protected:
    cocos2d::CCPoint m_point;
};