#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;

class SetupRandTriggerPopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupRandTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onTargetID2Arrow(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void updateChance();
    void updateChanceLabel(bool decimals);
    void updateTargetID();
    void updateTargetID2();
    void updateTextInputLabel();
    void updateTextInputLabel2();

    ~SetupRandTriggerPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_groupID1 = 0;
    int m_groupID2 = 0;
    float m_chancePercent = 0.0f;
    CCTextInputNode* m_groupID1Input = nullptr;
    CCTextInputNode* m_groupID2Input = nullptr;
    CCTextInputNode* m_chanceInput = nullptr;
    bool m_disableTextDelegate = false;
    Slider* m_chanceSlider = nullptr;
};