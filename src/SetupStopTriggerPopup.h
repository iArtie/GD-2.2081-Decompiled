#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupStopTriggerPopup : public SetupTriggerPopup {
public:
    static SetupStopTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void onClose(cocos2d::CCObject* sender);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);

    ~SetupStopTriggerPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
};