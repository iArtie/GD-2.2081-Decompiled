#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class RotateGameplayGameObject;

class SetupRotateGameplayPopup : public SetupTriggerPopup {
public:
    static SetupRotateGameplayPopup* create(RotateGameplayGameObject* object, cocos2d::CCArray* objects);
    bool init(RotateGameplayGameObject* object, cocos2d::CCArray* objects);

    void onPlusButton(cocos2d::CCObject* sender);
    void valueDidChange(int tag, float value);

    ~SetupRotateGameplayPopup();
};