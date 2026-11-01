#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "TextInputDelegate.h"
#include "Slider.h"
#include "SliderThumb.h"
#include "CCTextInputNode.h"
#include "InfoAlertButton.h"
#include <string>

class ConfigureValuePopupDelegate;

class ConfigureValuePopup : public FLAlertLayer, public TextInputDelegate {
public:
    static ConfigureValuePopup* create(float value, float min, float max, 
                                       const std::string& title, const std::string& description, 
                                       int type, bool showSlider, bool showArrows);
    bool init(float value, float min, float max, 
              const std::string& title, const std::string& description, 
              int type, bool showSlider, bool showArrows);
    
    void addInfoAlertButton(const std::string& title, const std::string& description);
    void keyBackClicked() override;
    void onArrow(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override {}
    void updateTextInputLabel();
    virtual ~ConfigureValuePopup();

    void setDelegate(ConfigureValuePopupDelegate* delegate) { m_delegate = delegate; }
    float getValue() const { return m_value; }
    void setValue(float value);

protected:
    float m_value;
    float m_minimum;
    float m_maximum;
    int m_type;
    bool m_enableDelegate;
    bool m_disableTextDelegate;
    
    CCTextInputNode* m_input;
    Slider* m_slider;
    cocos2d::CCMenu* m_buttonMenu;
    ConfigureValuePopupDelegate* m_delegate;
};