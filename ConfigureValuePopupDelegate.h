#pragma once

class ConfigureValuePopup;

class ConfigureValuePopupDelegate {
public:
    virtual ~ConfigureValuePopupDelegate() {}
    virtual void valuePopupClosed(ConfigureValuePopup* popup, float value) = 0;
};