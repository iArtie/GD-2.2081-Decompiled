#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class CameraTriggerGameObject;

class SetupStaticCameraPopup : public SetupTriggerPopup {
public:
    static SetupStaticCameraPopup* create(CameraTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(CameraTriggerGameObject* object, cocos2d::CCArray* objects);

    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    void updateState();
    void valueDidChange(int tag, float value) override;

    ~SetupStaticCameraPopup();
};