// GJGradientLayer.cpp
#include "GJGradientLayer.h"

GJGradientLayer* GJGradientLayer::create() {
    GJGradientLayer* instance = new GJGradientLayer();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GJGradientLayer::init() {
    cocos2d::ccColor4B color = cocos2d::ccc4(255, 255, 255, 255);
    cocos2d::CCPoint vector = cocos2d::CCPoint(-1.7014e38f, 0.0f);
    
    if (!cocos2d::CCLayerGradient::initWithColor(color, vector)) {
        return false;
    }
    
    setAnchorPoint(cocos2d::CCPointZero);
    return true;
}

GJGradientLayer::~GJGradientLayer() {
    // Base class handles cleanup
}