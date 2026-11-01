#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EnterEffectObject;
class EffectGameObject;
class CCMenuItemToggler;

class SetupEnterTriggerPopup : public SetupTriggerPopup {
public:
    static SetupEnterTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);
    bool init(EnterEffectObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onEnterType(cocos2d::CCObject* sender);

    ~SetupEnterTriggerPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_enterType = -1;
    CCMenuItemToggler* m_enterOnlyToggler = nullptr;
    CCMenuItemToggler* m_exitOnlyToggler = nullptr;
    bool m_disableTextDelegate = false;
};