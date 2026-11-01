// GJCommentListLayer.cpp
#include "Layers/GJCommentListLayer.h"

GJCommentListLayer* GJCommentListLayer::create(BoomListView* list, const char* title,
                                               cocos2d::ccColor4B color, float width,
                                               float height, bool useCustomColor) {
    GJCommentListLayer* instance = new GJCommentListLayer();
    if (instance && instance->init(list, title, color, width, height, useCustomColor)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GJCommentListLayer::init(BoomListView* list, const char* title,
                              cocos2d::ccColor4B color, float width,
                              float height, bool useCustomColor) {
    if (!cocos2d::CCLayerColor::initWithColor(color, width, height)) {
        return false;
    }
    
    // Register touch priority
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->registerForcePrio(this, 2);
    
    // Load top and side textures based on custom color flag
    std::string topTexture = useCustomColor ? "GJ_commentTop2_001.png" : "GJ_commentTop_001.png";
    std::string sideTexture = useCustomColor ? "GJ_commentSide2_001.png" : "GJ_commentSide_001.png";
    
    // Top border (top-left)
    cocos2d::CCSprite* topLeft = cocos2d::CCSprite::createWithSpriteFrameName(topTexture);
    topLeft->setFlipY(true);
    topLeft->setPosition(cocos2d::CCPoint(width * 0.5f, 4.0f));
    addChild(topLeft, 10);
    
    // Top border (top-right, flipped)
    cocos2d::CCSprite* topRight = cocos2d::CCSprite::createWithSpriteFrameName(topTexture);
    topRight->setPosition(cocos2d::CCPoint(width * 0.5f, height - 4.0f));
    topRight->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    addChild(topRight, 10);
    
    // Side border (left)
    cocos2d::CCSprite* sideLeft = cocos2d::CCSprite::createWithSpriteFrameName(sideTexture);
    sideLeft->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(sideLeft, 9);
    sideLeft->setPosition(cocos2d::CCPoint(-7.0f, 0.0f));
    sideLeft->setScaleY((height - 5.0f) / sideLeft->getContentSize().height);
    
    // Side border (right)
    cocos2d::CCSprite* sideRight = cocos2d::CCSprite::createWithSpriteFrameName(sideTexture);
    sideRight->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    addChild(sideRight, 9);
    sideRight->setPosition(cocos2d::CCPoint(width + 7.0f, 0.0f));
    sideRight->setScaleY(sideLeft->getScaleY());
    sideRight->setFlipX(true);
    
    // Add the list
    if (list) {
        m_list = list;
        list->retain();
        addChild(list, 6);
    }
    
    return true;
}

GJCommentListLayer::~GJCommentListLayer() {
    if (m_list) {
        m_list->release();
    }
}