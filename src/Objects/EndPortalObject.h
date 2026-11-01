#pragma once

#include "Objects/GameObject.h"
#include "Platform/cocos2d.h"

class GJBaseGameLayer;

class EndPortalObject : public GameObject {
public:
    static EndPortalObject* create();
    
    cocos2d::CCPoint getSpawnPos();
    bool init() override;
    void setPosition(const cocos2d::CCPoint& position) override;
    void setVisible(bool visible) override;
    void triggerObject(GJBaseGameLayer* layer);
    void updateEndPos(bool updateParticle);
    
    virtual ~EndPortalObject();

private:
    cocos2d::CCPoint m_startPosition;
    bool m_flippedX;
    bool m_startPosHeightRelated;
    bool m_isInvisible;
    bool m_unk3F8;
    int m_objectType;
    cocos2d::CCPoint m_particleOffset;
    cocos2d::CCSprite* m_gradientBar;
    bool m_unk370;
    cocos2d::CCParticleSystemQuad* m_particle;
};