#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "EffectGameObject.h"
#include <string>

class ShaderGameObject : public EffectGameObject {
public:
    static ShaderGameObject* create(const char* frame);
    bool init(const char* frame);

    void customSetup() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;

    ~ShaderGameObject();

    int m_objectID = 0;
    float m_waveWidth = 0.0f;
    float m_targetX = 0.0f;
    float m_targetY = 0.0f;
    float m_strength = 0.0f;
    float m_speed = 0.0f;
    float m_outer = 0.0f;
    float m_timeOff = 0.0f;
    float m_screenOffsetX = 0.0f;
    float m_screenOffsetY = 0.0f;
    bool m_flip = false;
    float m_fadeIn = 0.0f;
    float m_fadeOut = 0.0f;
    bool m_invert = false;
    bool m_rotate = false;
    bool m_dual = false;
    bool m_useX = false;
    bool m_useY = false;
    bool m_snapGrid = false;
    bool m_hardEdges = false;
    bool m_disableAll = false;
    float m_inner = 0.0f;
    float m_maxSize = 0.0f;
    float m_duration = 0.0f;
    int m_easingType = 0;
    float m_easingRate = 0.0f;
    int m_targetGroupID = 0;
    int m_centerGroupID = 0;
    int m_targetPlayer1 = 0;
    int m_targetPlayer2 = 0;
    int m_followCPP = 0;
    int m_zLayerMin = 0;
    int m_zLayerMax = 0;
    bool m_animate = false;
    bool m_relative = false;
    bool m_editorDisabled = false;
};