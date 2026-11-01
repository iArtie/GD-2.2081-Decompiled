#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;
class CCTextInputNode;

class SetupAnimationPopup : public SetupTriggerPopup {
public:
    static SetupAnimationPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects) override;

    void determineStartValues() override;
    void textChanged(CCTextInputNode* node) override;

    void onAnimationIDArrow(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender) override;
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void updateAnimationID();
    void updateAnimationTextInputLabel();
    void updateTargetID();
    void updateTextInputLabel();

    ~SetupAnimationPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_targetID = 0;
    int m_animationID = 0;
    CCTextInputNode* m_targetIDInput = nullptr;
    CCTextInputNode* m_animationIDInput = nullptr;
    bool m_disableTextDelegate = false;
};