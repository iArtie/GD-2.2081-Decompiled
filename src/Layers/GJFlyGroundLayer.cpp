// GJFlyGroundLayer.cpp
#include "Layers/GJFlyGroundLayer.h"

GJFlyGroundLayer* GJFlyGroundLayer::create() {
    GJFlyGroundLayer* instance = new GJFlyGroundLayer();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GJFlyGroundLayer::init() {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }
    m_showGround = false;
    return true;
}

GJFlyGroundLayer::~GJFlyGroundLayer() {
    // Base class handles cleanup
}