#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"
#include "TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class CCMenuItemToggler;

class SetupCountTriggerPopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupCountTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onEnableGroup(cocos2d::CCObject* sender);
    void onItemIDArrow(cocos2d::CCObject* sender);
    void onMultiActivate(cocos2d::CCObject* sender);
    void onTargetCountArrow(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void updateCountTextInputLabel();
    void updateItemID();
    void updateItemIDInputLabel();
    void updateTargetCount();
    void updateTargetID();
    void updateTargetIDInputLabel();

    ~SetupCountTriggerPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_activateGroup = true;
    bool m_multiActivate = true;
    int m_itemID = 0;
    int m_targetID = 0;
    int m_targetCount = 0;
    CCTextInputNode* m_itemIDInput = nullptr;
    CCTextInputNode* m_targetIDInput = nullptr;
    CCTextInputNode* m_targetCountInput = nullptr;
    bool m_disableTextDelegate = false;
    bool m_touchTriggered = false;
    CCMenuItemToggler* m_touchToggle = nullptr;
    bool m_spawnTriggered = false;
    CCMenuItemToggler* m_spawnToggle = nullptr;
    bool m_multiTriggered = false;
    cocos2d::CCNode* m_multiTriggerContainer = nullptr;
};