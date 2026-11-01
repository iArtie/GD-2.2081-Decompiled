#pragma once

#include "cocos2d.h"
#include "SetupTriggerPopup.h"
#include "TextInputDelegate.h"
#include "ColorSelectDelegate.h"
#include "GJSpecialColorSelectDelegate.h"
#include <string>

class ColorAction;
class EffectGameObject;
class GJSpecialColorSelect;
class ConfigureHSVWidget;
class Slider;
class CCTextInputNode;

class ColorSelectPopup : public SetupTriggerPopup, public TextInputDelegate, public GJSpecialColorSelectDelegate {
public:
    static ColorSelectPopup* create(ColorAction* action);

    void closeColorSelect(cocos2d::CCObject* sender);
    void colorSelectClosed(GJSpecialColorSelect* select, int id);
    void colorValueChanged(cocos2d::ccColor3B color);
    void determineStartValues();
    cocos2d::ccColor3B getColorValue();
    static cocos2d::ccColor3B hexToColor(std::string hex);
    void keyBackClicked();
    void onCopy(cocos2d::CCObject* sender);
    void onCopyOpacity(cocos2d::CCObject* sender);
    void onDefault(cocos2d::CCObject* sender);
    void onHSVLegacyMode(cocos2d::CCObject* sender);
    void onPaste(cocos2d::CCObject* sender);
    void onPlayerColor1(cocos2d::CCObject* sender);
    void onPlayerColor2(cocos2d::CCObject* sender);
    void onSelectSpecialColor(cocos2d::CCObject* sender);
    void onTintGround(cocos2d::CCObject* sender);
    void onToggleHSVMode(cocos2d::CCObject* sender);
    void onToggleTintMode(cocos2d::CCObject* sender);
    void onUpdateCopyColor(cocos2d::CCObject* sender);
    void onUpdateCustomColor(cocos2d::CCObject* sender);
    void show();
    void sliderChanged(cocos2d::CCObject* sender);
    virtual void textChanged(CCTextInputNode* node) override;
    void updateColorLabels();
    void updateColorValue();
    void updateCopyColor();
    void updateCopyColorTextInputLabel();
    void updateCustomColorIdx();
    void updateDurLabel();
    void updateDuration();
    void updateHSVMode();
    void updateHSVValue();
    void updateOpacity();
    void updateOpacityLabel();
    void updateTextInputLabel();

    void setDelegate(ColorSelectDelegate* delegate) { m_delegate = delegate; }

    ~ColorSelectPopup();

private:
    EffectGameObject* m_gameObject;
    cocos2d::CCArray* m_gameObjects;
    bool m_blending;
    bool m_copyOpacity;
    bool m_backgroundTrigger;
    bool m_tintGround;
    bool m_legacyHSV;
    int m_playerColor;
    cocos2d::ccColor3B m_currentColor;
    int m_colorID;
    int m_copyColorID;
    float m_opacity;
    HSV m_hsv;
    bool m_colorTrigger;
    bool m_newColorTrigger;
    float m_fadeTime;
    ColorAction* m_colorAction;
    bool m_noChannel;
    bool m_showCopyObjects;
    CCMenuItemToggler* m_playerColor2Toggle;
    CCMenuItemToggler* m_playerColor1Toggle;
    bool m_disableTextDelegate;
    CCTextInputNode* m_fadeTimeInput;
    cocos2d::CCLabelBMFont* m_opacityLabel;
    bool m_initializing;
    Slider* m_opacitySlider;
    cocos2d::CCNode* m_colorPicker;
    cocos2d::ccColor3B m_originalColor;
    CCTextInputNode* m_colorInput;
    cocos2d::CCArray* m_colorObjects;
    cocos2d::CCArray* m_copyObjects;
    CCTextInputNode* m_copyColorInput;
    CCTextInputNode* m_rInput;
    CCTextInputNode* m_gInput;
    CCTextInputNode* m_bInput;
    CCTextInputNode* m_hexInput;
    ConfigureHSVWidget* m_hsvWidget;
    ColorSelectDelegate* m_delegate;
    cocos2d::CCSprite* m_currentColorSprite;
    cocos2d::CCSprite* m_originalColorSprite;
    Slider* m_fadeTimeSlider;
};