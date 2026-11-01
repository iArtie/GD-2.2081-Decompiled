#pragma once

#include "Platform/cocos2d.h"
#include <string>

class ConfigureHSVWidget;
class HSVWidgetDelegate;

class HSVWidgetPopup : public FLAlertLayer {
public:
    static HSVWidgetPopup* create(cocos2d::ccHSVValue hsv, HSVWidgetDelegate* delegate, const std::string& title);
    bool init(cocos2d::ccHSVValue hsv, HSVWidgetDelegate* delegate, const std::string& title);
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    virtual ~HSVWidgetPopup();

private:
    ConfigureHSVWidget* m_widget;
    HSVWidgetDelegate* m_delegate;
};