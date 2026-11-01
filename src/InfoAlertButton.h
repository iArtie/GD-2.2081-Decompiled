#pragma once

#include "cocos2d.h"
#include <string>

class InfoAlertButton : public CCMenuItemSpriteExtra {
public:
    void activate();
    static InfoAlertButton* create(const std::string& title, const std::string& desc, float spriteScale);
    bool init(const std::string& title, const std::string& desc, float spriteScale);
    virtual ~InfoAlertButton();

private:
    std::string m_title;
    std::string m_description;
    bool m_bEnabled;
    bool m_scroll;
    float m_textScale;
};