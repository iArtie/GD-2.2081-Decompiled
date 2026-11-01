#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/TextInputDelegate.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"
#include "Cocos/CCTextInputNode.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

class HSVWidgetDelegate;

class ConfigureHSVWidget : public cocos2d::CCNode, public TextInputDelegate {
public:
    static ConfigureHSVWidget* create(cocos2d::ccHSVValue hsv, bool noBackground, bool addInputs);
    bool init(cocos2d::ccHSVValue hsv, bool noBackground, bool addInputs);
    void onClose();
    void onResetHSV(cocos2d::CCObject* sender);
    void onToggleSConst(cocos2d::CCObject* sender);
    void onToggleVConst(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    void updateLabels();
    void updateSliders();
    virtual ~ConfigureHSVWidget();

    void setDelegate(HSVWidgetDelegate* delegate) { m_delegate = delegate; }
    cocos2d::ccHSVValue getHSV() const { return m_hsv; }

protected:
    cocos2d::ccHSVValue m_hsv;
    cocos2d::CCDictionary* m_inputs;
    Slider* m_hueSlider;
    Slider* m_saturationSlider;
    Slider* m_brightnessSlider;
    cocos2d::CCLabelBMFont* m_hueLabel;
    cocos2d::CCLabelBMFont* m_saturationLabel;
    cocos2d::CCLabelBMFont* m_brightnessLabel;
    HSVWidgetDelegate* m_delegate;
    bool m_updating;
    bool m_addInputs;
    bool m_mixed;
};