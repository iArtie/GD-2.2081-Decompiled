#pragma once

#include "cocos2d.h"
#include <vector>

class FontObject : public cocos2d::CCObject {
public:
    static FontObject* createWithConfigFile(const char* font, float scale);
    
    int getFontWidth(int character);
    bool initWithConfigFile(const char* font, float scale);
    void parseConfigFile(const char* font, float scale);
    
    virtual ~FontObject();

private:
    std::vector<int> m_fontWidths;
};