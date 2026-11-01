#pragma once

#include "cocos2d.h"
#include "CCSpriteWithHue.h"
#include <string>

class CCSpriteGrayscale : public CCSpriteWithHue {
public:
    static CCSpriteGrayscale* create(const std::string* file);
    static CCSpriteGrayscale* createWithSpriteFrame(cocos2d::CCSpriteFrame* frame);
    static CCSpriteGrayscale* createWithSpriteFrameName(const std::string* frameName);
    static CCSpriteGrayscale* createWithTexture(cocos2d::CCTexture2D* texture);

    std::string getShaderName();
    const char* shaderBody();

    ~CCSpriteGrayscale();
};