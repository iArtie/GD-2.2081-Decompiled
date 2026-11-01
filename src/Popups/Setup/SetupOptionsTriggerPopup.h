#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include <string>

class GameOptionsTrigger;

class SetupOptionsTriggerPopup : public SetupTriggerPopup {
public:
    static SetupOptionsTriggerPopup* create(GameOptionsTrigger* object, cocos2d::CCArray* objects);
    bool init(GameOptionsTrigger* object, cocos2d::CCArray* objects);

    void addOption(int property, std::string label);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);

    ~SetupOptionsTriggerPopup();

protected:
    cocos2d::CCPoint m_togglePosition;
    int m_togglePage = 0;
};