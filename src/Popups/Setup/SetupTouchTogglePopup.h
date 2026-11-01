#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class CCMenuItemToggler;

class SetupTouchTogglePopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupTouchTogglePopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onControlMode(cocos2d::CCObject* sender);
    void onDualTouchMode(cocos2d::CCObject* sender);
    void onHoldMode(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void onTouchMode(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node) override;
    void updateTargetID();
    void updateTextInputLabel();

    ~SetupTouchTogglePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_groupID = 0;
    int m_toggleMode = 0;
    int m_playerMode = 0;
    bool m_holdMode = true;
    bool m_dualMode = true;
    CCMenuItemToggler* m_toggleOnToggler = nullptr;
    CCMenuItemToggler* m_toggleOffToggler = nullptr;
    CCMenuItemToggler* m_p1OnlyToggler = nullptr;
    CCMenuItemToggler* m_p2OnlyToggler = nullptr;
    CCTextInputNode* m_groupIDInput = nullptr;
    bool m_disableTextDelegate = false;
};