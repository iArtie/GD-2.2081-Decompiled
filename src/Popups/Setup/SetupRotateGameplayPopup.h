#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class RotateGameplayGameObject;

class SetupRotateGameplayPopup : public SetupTriggerPopup {
public:
    static SetupRotateGameplayPopup* create(RotateGameplayGameObject* object, cocos2d::CCArray* objects);
    bool init(RotateGameplayGameObject* object, cocos2d::CCArray* objects);

    void onPlusButton(cocos2d::CCObject* sender);
    void valueDidChange(int tag, float value);

    ~SetupRotateGameplayPopup();
};