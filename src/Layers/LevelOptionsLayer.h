#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJOptionsLayer.h"
#include "Utils/GJEnums.h"

class LevelSettingsObject;
class CCMenuItemToggler;

class LevelOptionsLayer : public GJOptionsLayer {
public:
    static LevelOptionsLayer* create(LevelSettingsObject* object);
    void didToggle(int tag) override;
    float getValue(int tag) override;
    bool init(LevelSettingsObject* object);
    void onSettings(cocos2d::CCObject* sender);
    void setupOptions() override;
    void valueDidChange(int tag, float value) override;
    ~LevelOptionsLayer();

protected:
    void addToggle(const char* title, int tag, bool initialValue, const char* info);
    LevelSettingsObject* m_settingsObject = nullptr;
};