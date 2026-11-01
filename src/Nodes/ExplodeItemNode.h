#pragma once

#include "Platform/cocos2d.h"

class ExplodeItemNode : public cocos2d::CCNode {
public:
    static ExplodeItemNode* create(cocos2d::CCRenderTexture* texture);
    
    bool init(cocos2d::CCRenderTexture* texture);
    void update(float dt) override;
    
    virtual ~ExplodeItemNode();

private:
    cocos2d::CCRenderTexture* m_renderTexture;
    cocos2d::CCArray* m_sprites;
    float m_floorHeight;
    bool m_noGravity;
};