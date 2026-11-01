#include "Editor/GJSmartBlockPreviewSprite.h"
#include "Utils/GameToolbox.h"

GJSmartBlockPreviewSprite* GJSmartBlockPreviewSprite::create() {
    auto sprite = new GJSmartBlockPreviewSprite();
    if (sprite && sprite->init()) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

void GJSmartBlockPreviewSprite::visit() {
    auto parent = getParent();
    auto grandParent = parent ? parent->getParent() : nullptr;
    
    if (grandParent && parent) {
        float scale = parent->getScale();
        float size = scale * 92.0f * grandParent->getScale();
        cocos2d::CCPoint pos = grandParent->convertToWorldSpace(parent->getPosition());
        
        cocos2d::CCRect clipRect(
            pos.x - size * 0.5f,
            pos.y - size * 0.5f,
            size,
            size
        );
        
        GameToolbox::preVisitWithClipRect(clipRect);
        cocos2d::CCNode::visit();
        GameToolbox::postClipVisit();
    } else {
        cocos2d::CCNode::visit();
    }
}

GJSmartBlockPreviewSprite::~GJSmartBlockPreviewSprite() {
}