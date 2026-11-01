#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "ColorSelectDelegate.h"
#include "TextInputDelegate.h"

class ColorAction;
class EffectGameObject;

class ColorSelectLiveOverlay : public FLAlertLayer, public TextInputDelegate {
public:
    static ColorSelectLiveOverlay* createWithActions(ColorAction* baseAction, ColorAction* detailAction);
    static ColorSelectLiveOverlay* createWithObject(EffectGameObject* object);

    void closeColorSelect(cocos2d::CCObject* sender);
    void colorValueChanged(cocos2d::ccColor3B color);
    void determineStartValues();
    cocos2d::ccColor3B getColorValue();
    void keyBackClicked();
    void onSelectTab(cocos2d::CCObject* sender);
    void show();
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void toggleControls(bool visible);
    void updateColorLabel();
    void updateColorValue();
    void updateOpacity();

    void setDelegate(ColorSelectDelegate* delegate) { m_delegate = delegate; }

    ~ColorSelectLiveOverlay();

private:
    bool m_closeButtonPressed;
    ColorSelectDelegate* m_delegate;
    int m_ZOrder;
    cocos2d::CCNode* m_scene;
    EffectGameObject* m_effectGameObject;
    cocos2d::ccColor3B m_targetColor;
    cocos2d::ccColor3B m_mainColor;
    float m_opacity;
    ColorAction* m_baseColorAction;
    ColorAction* m_detailColorAction;
    cocos2d::ccColor3B m_detailColor;
    bool m_updateColor;
    bool m_detailColorSelected;
    double m_hue;
    double m_saturation;
    double m_value;
    cocos2d::CCSprite* m_colorSprite;
    cocos2d::CCSprite* m_saturationSprite;
    cocos2d::CCSprite* m_valueSprite;
    cocos2d::CCSprite* m_opacitySprite;
    cocos2d::CCArray* m_barSprites;
    cocos2d::CCLabelBMFont* m_colorLabel;
    cocos2d::CCArray* m_12buttons;
    cocos2d::CCSprite* m_oldColorSprite;
};