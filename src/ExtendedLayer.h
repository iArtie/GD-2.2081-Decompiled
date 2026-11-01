#pragma once

#include "cocos2d.h"

class BoomScrollLayerDelegate;

class ExtendedLayer : public cocos2d::CCLayer {
public:
    static ExtendedLayer* create();
    
    void setPosition(const cocos2d::CCPoint& position) override;
    
    virtual ~ExtendedLayer();

private:
    BoomScrollLayerDelegate* m_delegate;
};