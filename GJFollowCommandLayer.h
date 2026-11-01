// GJFollowCommandLayer.h
#pragma once

#include "cocos2d.h"
#include "SetupTriggerPopup.h"
#include "TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;

class GJFollowCommandLayer : public SetupTriggerPopup, public TextInputDelegate {
public:
    static GJFollowCommandLayer* create(EffectGameObject* object, cocos2d::CCArray* objects);
    
    void determineStartValues();
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    void onClose(cocos2d::CCObject* sender) override;
    void onUpdateGroupID(cocos2d::CCObject* sender);
    void onUpdateGroupID2(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void sliderXModChanged(cocos2d::CCObject* sender);
    void sliderYModChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node) override;
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateTargetGroupID();
    void updateTargetGroupID2();
    void updateTextInputLabel();
    void updateTextInputLabel2();
    void updateXMod();
    void updateXModLabel();
    void updateYMod();
    void updateYModLabel();
    virtual ~GJFollowCommandLayer();

private:
    CCTextInputNode* m_moveTimeInput;
    CCTextInputNode* m_targetIDInput;
    CCTextInputNode* m_followIDInput;
    CCTextInputNode* m_xModInput;
    CCTextInputNode* m_yModInput;
    EffectGameObject* m_gameObject;
    int m_targetGroupID;
    int m_followGroupID;
    float m_moveTime;
    float m_xMod;
    float m_yMod;
    bool m_touchTriggered;
    bool m_spawnTriggered;
    bool m_multiTriggered;
    cocos2d::CCArray* m_gameObjects;
    bool m_disableTextDelegate;
    float m_height;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    float m_width;
    Slider* m_moveTimeSlider;
    Slider* m_xModSlider;
    Slider* m_yModSlider;
};