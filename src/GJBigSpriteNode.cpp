// GJBigSpriteNode.cpp
#include "GJBigSpriteNode.h"

GJBigSpriteNode* GJBigSpriteNode::create() {
    GJBigSpriteNode* instance = new GJBigSpriteNode();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

GJBigSpriteNode::GJBigSpriteNode() : m_sprite(nullptr), m_spriteVisible(false) {
    m_spriteRect = cocos2d::CCRectZero;
}

GJBigSpriteNode::~GJBigSpriteNode() {
    // m_spriteName destructor automatically handles string cleanup
    cocos2d::CCNode::~CCNode();
}