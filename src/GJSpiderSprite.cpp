#include "GJSpiderSprite.h"
#include "GJRobotSprite.h"

GJSpiderSprite* GJSpiderSprite::create(int frame) {
    auto sprite = new GJSpiderSprite();
    if (sprite && sprite->init(frame)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

bool GJSpiderSprite::init(int frame) {
    m_iconType = IconType::Spider;
    return GJRobotSprite::init(frame, "Spider");
}

GJSpiderSprite::~GJSpiderSprite() {
}