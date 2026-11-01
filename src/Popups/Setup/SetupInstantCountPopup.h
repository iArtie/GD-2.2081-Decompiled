#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class CountTriggerGameObject;
class EffectGameObject;
class CCTextInputNode;
class CCMenuItemToggler;

class SetupInstantCountPopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupInstantCountPopup* create(CountTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(CountTriggerGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onCountType(cocos2d::CCObject* sender);
    void onEnableGroup(cocos2d::CCObject* sender);
    void onTargetCountArrow(cocos2d::CCObject* sender);
    void onTargetID2Arrow(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void updateCountTextInputLabel();
    void updateItemID();
    void updateItemIDInputLabel();
    void updateTargetCount();
    void updateTargetID();
    void updateTargetIDInputLabel();

    ~SetupInstantCountPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_itemID = -1;
    int m_targetID = -1;
    int m_targetCount = 0;
    int m_mode = -1;
    bool m_activateGroup = true;
    CCTextInputNode* m_itemIDInput = nullptr;
    CCTextInputNode* m_targetIDInput = nullptr;
    CCTextInputNode* m_targetCountInput = nullptr;
    CCMenuItemToggler* m_equalsToggler = nullptr;
    CCMenuItemToggler* m_largerToggler = nullptr;
    CCMenuItemToggler* m_smallerToggler = nullptr;
    bool m_disableTextDelegate = false;
};