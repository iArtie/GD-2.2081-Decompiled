#pragma once

#include "cocos2d.h"

class ConfigureHSVWidget;
class HSVWidgetPopup;

class HSVWidgetDelegate {
public:
    virtual void hsvChanged(ConfigureHSVWidget* widget);
    virtual void hsvPopupClosed(HSVWidgetPopup* popup, cocos2d::ccHSVValue value);
    virtual ~HSVWidgetDelegate() = default;
};