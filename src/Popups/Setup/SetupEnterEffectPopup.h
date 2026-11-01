#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Editor/HSVWidgetDelegate.h"

class EnterEffectObject;
class EffectGameObject;
class CCMenuItemToggler;
class CCMenuItemSpriteExtra;

class SetupEnterEffectPopup : public SetupTriggerPopup, public HSVWidgetDelegate {
public:
    static SetupEnterEffectPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects, int id);
    bool init(EnterEffectObject* object, cocos2d::CCArray* objects, int id);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onEnterType(cocos2d::CCObject* sender);
    void onHSV(cocos2d::CCObject* sender);
    void onNextFreeEnterChannel(cocos2d::CCObject* sender);
    void onNextFreeEnterEffectID(cocos2d::CCObject* sender);
    float triggerSliderValueFromValue(int tag, float value);
    float triggerValueFromSliderValue(int tag, float value);
    void updateHSVButton();
    void updateInputNode(int tag, float value);
    void updateInputValue(int tag, float* value);
    void valueDidChange(int tag, float value);

    ~SetupEnterEffectPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_enterType = -1;
    int m_objectID = 0;
    cocos2d::ccHSVValue m_hsvValue;
    CCMenuItemToggler* m_enterOnlyToggler = nullptr;
    CCMenuItemToggler* m_exitOnlyToggler = nullptr;
    bool m_hsvChanged = false;
    CCMenuItemSpriteExtra* m_hsvButton = nullptr;
};