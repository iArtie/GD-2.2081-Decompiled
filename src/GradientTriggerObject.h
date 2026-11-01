#pragma once

#include <string>

class GJBaseGameLayer;

class GradientTriggerObject : public EffectGameObject {
public:
    static GradientTriggerObject* create();
    std::string getSaveString(GJBaseGameLayer* layer) override;
    bool init() override;
    virtual ~GradientTriggerObject();

    int m_blendingLayer;
    int m_blendingMode;
    int m_upBottomLeftID;
    int m_downBottomRightID;
    int m_leftTopLeftID;
    int m_rightTopRightID;
    int m_vertexMode;
    int m_disable;
    int m_disableAll;
    int m_gradientID;
    float m_previewOpacity;
};