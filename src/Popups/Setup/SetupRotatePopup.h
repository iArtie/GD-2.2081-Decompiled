#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EnhancedGameObject;
class EffectGameObject;

class SetupRotatePopup : public SetupTriggerPopup {
public:
    static SetupRotatePopup* create(EnhancedGameObject* object, cocos2d::CCArray* objects);
    bool init(EnhancedGameObject* object, cocos2d::CCArray* objects);

    void onClose(cocos2d::CCObject* sender);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);

    ~SetupRotatePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
};