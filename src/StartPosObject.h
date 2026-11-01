#pragma once

#include "cocos2d.h"
#include "EffectGameObject.h"
#include <string>

class GJBaseGameLayer;
class LevelSettingsObject;

class StartPosObject : public EffectGameObject {
public:
    static StartPosObject* create();
    bool init() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    void loadSettingsFromString(std::string objectString);
    void setSettings(LevelSettingsObject* settings);
    virtual ~StartPosObject();

protected:
    LevelSettingsObject* m_startSettings = nullptr;
};