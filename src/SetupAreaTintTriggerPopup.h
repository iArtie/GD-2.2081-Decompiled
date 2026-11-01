#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupAreaMoveTriggerPopup.h"
#include "HSVWidgetDelegate.h"

class EnterEffectObject;
class EffectGameObject;
class HSVWidgetPopup;
class CCMenuItemSpriteExtra;

class SetupAreaTintTriggerPopup : public SetupAreaMoveTriggerPopup, public HSVWidgetDelegate {
public:
    static SetupAreaTintTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);
    bool init(EnterEffectObject* object, cocos2d::CCArray* objects) override;

    void determineStartValues() override;
    void onClose(cocos2d::CCObject* sender) override;
    void valueDidChange(int tag, float value) override;

    void onHSV(cocos2d::CCObject* sender);
    void updateHSVButton();
    void hsvPopupClosed(HSVWidgetPopup* popup, cocos2d::ccHSVValue value) override;

    ~SetupAreaTintTriggerPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    cocos2d::ccHSVValue m_hsvValue;
    bool m_hsvChanged = false;
    CCMenuItemSpriteExtra* m_hsvButton = nullptr;
};