#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupObjectTogglePopup : public SetupTriggerPopup {
public:
    void onClose(cocos2d::CCObject* sender);
    void valueDidChange(int tag, float value);
    ~SetupObjectTogglePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
};