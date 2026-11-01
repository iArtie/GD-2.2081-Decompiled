#pragma once

#include "Platform/cocos2d.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;

class GJPFollowCommandLayer : public SetupTriggerPopup {
public:
    static GJPFollowCommandLayer* create(EffectGameObject* object, cocos2d::CCArray* objects);
    void determineStartValues();
    bool init(EffectGameObject* object, cocos2d::CCArray* objects) override;
    void onClose(cocos2d::CCObject* sender) override;
    void onUpdateGroupID(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void sliderXModChanged(cocos2d::CCObject* sender);
    void sliderYModChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node) override;
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateMaxSpeed();
    void updateMaxSpeedLabel();
    void updateOffsetLabel();
    void updatePlayerOffset();
    void updateTargetGroupID();
    void updateTextInputLabel();
    void updateXMod();
    void updateXModLabel();
    void updateYMod();
    void updateYModLabel();
    virtual ~GJPFollowCommandLayer();

private:
    CCTextInputNode* m_moveTimeInput;
    CCTextInputNode* m_targetIDInput;
    CCTextInputNode* m_speedInput;
    CCTextInputNode* m_delayInput;
    CCTextInputNode* m_offsetInput;
    CCTextInputNode* m_maxSpeedInput;
    EffectGameObject* m_gameObject;
    int m_targetGroupID;
    int m_offset;
    float m_moveTime;
    float m_speed;
    float m_delay;
    float m_maxSpeed;
    bool m_touchTriggered;
    bool m_spawnTriggered;
    bool m_multiTriggered;
    cocos2d::CCArray* m_gameObjects;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    Slider* m_moveTimeSlider;
    Slider* m_speedSlider;
    Slider* m_delaySlider;
    bool m_disableTextDelegate;
};