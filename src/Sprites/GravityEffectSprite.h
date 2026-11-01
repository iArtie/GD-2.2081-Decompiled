#pragma once

#include "Platform/cocos2d.h"

class GravityEffectSprite : public cocos2d::CCSprite {
public:
    static GravityEffectSprite* create();
    void draw() override;
    bool init() override;
    virtual ~GravityEffectSprite();

private:
    cocos2d::CCSpriteBatchNode* m_batchNode;
};