#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

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