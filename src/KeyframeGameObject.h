#pragma once

#include "cocos2d.h"
#include <string>

class GJBaseGameLayer;
class EditorUI;

class KeyframeGameObject : public EffectGameObject {
public:
    static KeyframeGameObject* create();
    std::string getSaveString(GJBaseGameLayer* layer) override;
    bool init() override;
    void setOpacity(unsigned char opacity) override;
    void updateShadowObjects(GJBaseGameLayer* layer, EditorUI* ui);
    virtual ~KeyframeGameObject();

    bool m_referenceOnly;
    cocos2d::CCArray* m_shadowObjects;
    cocos2d::CCSprite* m_previewSprite;
    bool m_previewArt;
    int m_targetGroupID;
    bool m_isObjectRectDirty;
    int m_keyframeGroup;
    int m_keyframeIndex;
    int m_easingType;
    float m_easingRate;
    float m_duration;
    int m_centerGroupID;
    int m_proximity;
    float m_spawnDelay;
    int m_curve;
    int m_timeMode;
    int m_closeLoop;
    int m_autoLayer;
    int m_direction;
    int m_revolutions;
    float m_lineOpacity;
    float m_width;
    float m_height;
    cocos2d::CCPoint m_customBoxOffset;
    bool m_updateCustomContentSize;
};