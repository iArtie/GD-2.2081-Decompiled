#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"
#include "TextInputDelegate.h"

class CameraTriggerGameObject;
class EffectGameObject;
class CCTextInputNode;

class SetupCameraEdgePopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupCameraEdgePopup* create(CameraTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(CameraTriggerGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onCameraEdge(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void onUnlockEdge(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void updateTargetID();
    void updateTextInputLabel();

    ~SetupCameraEdgePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_targetID = 0;
    int m_edgeDirection = 0;
    cocos2d::CCArray* m_directionToggles = nullptr;
    CCTextInputNode* m_targetIDInput = nullptr;
    bool m_disableTextDelegate = false;
};