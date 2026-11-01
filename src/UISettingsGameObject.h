#pragma once

#include <string>
#include "EffectGameObject.h"

class GJBaseGameLayer;

class UISettingsGameObject : public EffectGameObject {
public:
    static UISettingsGameObject* create();
    bool init() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;

    ~UISettingsGameObject();

protected:
    int m_targetGroupID = 0;
    int m_centerGroupID = 0;
    int m_xRef = 0;
    int m_yRef = 0;
    int m_xRelative = 0;
    int m_yRelative = 0;
};