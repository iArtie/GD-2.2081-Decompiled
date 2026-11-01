#pragma once

#include <string>
#include <vector>

#include "Objects/EffectGameObject.h"

class RingObject : public EffectGameObject {
public:
    static RingObject* create(const char* frame);

    bool init(const char* frame) override;

    void customObjectSetup(std::vector<std::string>* values, std::vector<void*>* exists) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;

    void powerOnObject(int state) override;
    void resetObject() override;
    void setRScale(float scale) override;
    void setScale(float scale) override;
    bool shouldDrawEditorHitbox() override;

    void spawnCircle();
    void triggerActivated(float xPosition);

    virtual ~RingObject();

protected:
    int m_targetGroupID = 0;
    int m_activateGroup = 0;
    bool m_claimTouch = false;
    bool m_isSpawnOnly = false;
    bool m_hasNoEffects = false;
    bool m_isRingPoweredOn = false;
    bool m_customGlowColor = true;
    bool m_isTouchTriggered = true;
};