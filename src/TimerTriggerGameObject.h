#pragma once

#include "cocos2d.h"
#include "EffectGameObject.h"
#include <string>

class GJBaseGameLayer;

class TimerTriggerGameObject : public EffectGameObject {
public:
    static TimerTriggerGameObject* create(const char* frame);
    bool init(const char* frame) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    virtual ~TimerTriggerGameObject();

protected:
    int m_itemID = 0;
    int m_controlType = 0;
    double m_startTime = 0.0;
    double m_targetTime = 0.0;
    bool m_stopTimeEnabled = false;
    bool m_dontOverride = false;
    bool m_ignoreTimeWarp = false;
    double m_timeMod = 0.0;
    bool m_startPaused = false;
    int m_targetGroupID = 0;
    bool m_multiActivate = false;
};