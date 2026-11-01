#pragma once

#include "Platform/cocos2d.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;
class ConfigureValuePopup;

enum class EasingType {
    Linear = 0,
    EaseIn = 1,
    EaseOut = 2,
    EaseInOut = 3,
    BackIn = 4,
    BackOut = 5,
    BackInOut = 6,
    BounceIn = 7,
    BounceOut = 8,
    BounceInOut = 9,
    ElasticIn = 10,
    ElasticOut = 11,
    ElasticInOut = 12
};

class GJRotateCommandLayer : public SetupTriggerPopup {
public:
    static GJRotateCommandLayer* create(EffectGameObject* object, cocos2d::CCArray* objects);
    void determineStartValues();
    bool init(EffectGameObject* object, cocos2d::CCArray* objects) override;
    void onClose(cocos2d::CCObject* sender) override;
    void onEasing(cocos2d::CCObject* sender);
    void onEasingRate(cocos2d::CCObject* sender);
    void onFollowRotation(cocos2d::CCObject* sender);
    void onLockRotation(cocos2d::CCObject* sender);
    void onUpdateGroupID(cocos2d::CCObject* sender);
    void onUpdateGroupID2(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void sliderDegreesChanged(cocos2d::CCObject* sender);
    void sliderTimesChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node) override;
    void toggleEasingRateVisibility();
    void updateCommandDegrees();
    void updateCommandTimes();
    void updateDegreesLabel();
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateEasingLabel();
    void updateEasingRateLabel();
    void updateMoveCommandEasing();
    void updateMoveCommandEasingRate();
    void updateTargetGroupID();
    void updateTargetGroupID2();
    void updateTextInputLabel();
    void updateTextInputLabel2();
    void updateTimesLabel();
    void valuePopupClosed(ConfigureValuePopup* popup, float value);
    virtual ~GJRotateCommandLayer();

private:
    EffectGameObject* m_gameObject;
    EasingType m_rotateEasingType;
    int m_times360;
    int m_targetGroupID;
    int m_centerGroupID;
    float m_moveTime;
    float m_degrees;
    float m_rotateEasingRate;
    bool m_touchTriggered;
    bool m_spawnTriggered;
    bool m_lockObjectRotation;
    bool m_multiTriggered;
    bool m_followP1;
    bool m_followP2;
    cocos2d::CCArray* m_gameObjects;
    CCTextInputNode* m_moveTimeInput;
    CCTextInputNode* m_targetIDInput;
    CCTextInputNode* m_centerIDInput;
    CCTextInputNode* m_degreesInput;
    CCTextInputNode* m_timesInput;
    CCMenuItemToggler* m_followP1Toggler;
    CCMenuItemToggler* m_followP2Toggler;
    cocos2d::CCLabelBMFont* m_rotateEasingRateLabel;
    CCMenuItemSpriteExtra* m_rotateEasingRateButton;
    cocos2d::CCLabelBMFont* m_rotateEasingLabel;
    bool m_disableTextChanged;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    Slider* m_moveTimeSlider;
    Slider* m_degreesSlider;
    Slider* m_timesSlider;
};