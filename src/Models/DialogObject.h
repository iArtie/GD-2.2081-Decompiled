#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include <string>

class DialogObject : public cocos2d::CCObject {
public:
    static DialogObject* create(std::string character, std::string text, int characterFrame, float textScale, bool unskippable, cocos2d::ccColor3B nameColor);
    bool init(std::string character, std::string text, int characterFrame, float textScale, bool unskippable, cocos2d::ccColor3B nameColor);
    
    virtual ~DialogObject();

    std::string m_character;
    std::string m_text;
    int m_characterFrame;
    float m_textScale;
    bool m_unskippable;
    cocos2d::ccColor3B m_nameColor;
};