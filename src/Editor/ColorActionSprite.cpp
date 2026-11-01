#include "Editor/ColorActionSprite.h"
#include "Platform/cocos2d.h"

ColorActionSprite* ColorActionSprite::create() {
    ColorActionSprite* sprite = new ColorActionSprite();
    if (sprite && sprite->init()) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

bool ColorActionSprite::init() {
    if (!cocos2d::CCNode::init()) {
        return false;
    }

    m_copyColor.r = 255;
    m_copyColor.g = 255;
    m_copyColor.b = 255;

    m_color.r = 255;
    m_color.g = 255;
    m_color.b = 255;

    m_opacity = 255.0f;

    return true;
}

ColorActionSprite::~ColorActionSprite() {
    // Destructor
}