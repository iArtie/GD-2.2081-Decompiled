#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "FLAlertLayer.h"
#include "TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class CCLabelBMFont;

class SetupGravityModPopup : public FLAlertLayer, public TextInputDelegate {
public:
    void determineStartValues();
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onItemIDArrow(cocos2d::CCObject* sender);
    void show() override;
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void updateItemID();
    void updateTextInputLabel();
    void updateValue();
    void updateValueLabel();

    ~SetupGravityModPopup();

protected:
    CCTextInputNode* m_itemIDInput = nullptr;
    cocos2d::CCNode* m_scene = nullptr;
    EffectGameObject* m_gameObject = nullptr;
    int m_itemID = 0;
    float m_gravityMod = 1.0f;
    cocos2d::CCArray* m_gameObjects = nullptr;
    CCLabelBMFont* m_gravityModLabel = nullptr;
    bool m_gravityTrigger = false;
    float m_minValue = 0.0f;
    float m_maxValue = 1.0f;
    bool m_disableDelegate = false;
};