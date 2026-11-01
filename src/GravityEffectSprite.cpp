#include "GravityEffectSprite.h"

GravityEffectSprite* GravityEffectSprite::create() {
    auto sprite = new GravityEffectSprite();
    if (sprite && sprite->init()) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

bool GravityEffectSprite::init() {
    if (!CCSprite::init()) {
        return false;
    }
    
    auto batchNode = cocos2d::CCSpriteBatchNode::create("gravityLine_001.png", 2);
    batchNode->setTag(1);
    addChild(batchNode);
    batchNode->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
    
    auto director = cocos2d::CCDirector::sharedDirector();
    float scale = director->getContentScaleFactor() * 480.0f / director->getScreenRight();
    float spacing = (director->getScreenRight() - 60.0f) / 22.0f;
    
    for (int i = 0; i < 22; ++i) {
        auto line = cocos2d::CCSprite::create("gravityLine_001.png");
        batchNode->addChild(line);
        
        float xPos = (30.0f - director->getScreenRight() * 0.5f) + i * spacing + (rand() * 4.6566e-10f) * spacing;
        float yOffset = (rand() * 4.6566e-10f * 2.0f - 1.0f) * 10.0f;
        line->setPosition(cocos2d::CCPoint(xPos, yOffset));
        
        float scaleX = (rand() * 4.6566e-10f * 4.0f + 2.0f) * scale;
        line->setScaleX(scaleX);
        
        float scaleY = director->getContentScaleFactor() * (rand() * 4.6566e-10f + 2.0f);
        line->setScaleY(scaleY);
        
        int opacity = static_cast<int>((rand() * 4.6566e-10f * 130.0f) + 50.0f);
        line->setOpacity(opacity);
    }
    
    return true;
}

void GravityEffectSprite::draw() {
}

GravityEffectSprite::~GravityEffectSprite() {
}