#pragma once

#include "Platform/cocos2d.h"
#include <string>

class TextAlertPopup : public cocos2d::CCNode {
public:
    static TextAlertPopup* create(std::string text, float delay, float scale, int opacity, std::string font);
    bool init(std::string text, float delay, float scale, int opacity, std::string font);
    virtual ~TextAlertPopup();

protected:
    int m_unknown1 = 0;
    int m_unknown2 = 0;
    cocos2d::CCLabelBMFont* m_label = nullptr;
};