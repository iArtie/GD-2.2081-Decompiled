#pragma once

#include "cocos2d.h"

class CCNodeContainer : public cocos2d::CCNode {
public:
    static CCNodeContainer* create();

    virtual void visit() override;

    ~CCNodeContainer();
};