#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;

class SetupEndPopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupEndPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onLockY(cocos2d::CCObject* sender);
    void onMultiActivate(cocos2d::CCObject* sender);
    void onReversed(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void updateTargetID();
    void updateTextInputLabel();

    ~SetupEndPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    bool m_multiActivate = false;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_reversed = true;
    bool m_lockY = true;
    int m_groupID = 0;
    CCTextInputNode* m_groupIDInput = nullptr;
    bool m_disableTextDelegate = false;
};