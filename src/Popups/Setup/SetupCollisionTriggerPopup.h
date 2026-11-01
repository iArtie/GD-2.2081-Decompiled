#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;

class SetupCollisionTriggerPopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupCollisionTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onActivateOnExit(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onEnableGroup(cocos2d::CCObject* sender);
    void onItemID2Arrow(cocos2d::CCObject* sender);
    void onItemIDArrow(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void onTargetP1(cocos2d::CCObject* sender);
    void onTargetP2(cocos2d::CCObject* sender);
    void onTargetPP(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void updateItemID();
    void updateItemID2();
    void updateItemID2InputLabel();
    void updateItemIDInputLabel();
    void updateTargetID();
    void updateTargetIDInputLabel();

    ~SetupCollisionTriggerPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_activateGroup = true;
    bool m_triggerOnExit = true;
    bool m_targetP1 = true;
    bool m_targetP2 = true;
    bool m_targetPP = true;
    int m_blockAID = -1;
    int m_blockBID = -1;
    int m_targetID = -1;
    CCTextInputNode* m_blockAInput = nullptr;
    CCTextInputNode* m_blockBInput = nullptr;
    CCTextInputNode* m_targetIDInput = nullptr;
    bool m_disableTextDelegate = false;
};