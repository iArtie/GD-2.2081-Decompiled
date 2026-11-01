#pragma once

#include "Platform/cocos2d.h"

class CCExtenderNode : public cocos2d::CCNode {
public:
    virtual void setOpacity(unsigned int opacity);
};