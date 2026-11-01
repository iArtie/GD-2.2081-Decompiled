#pragma once

#include "Platform/cocos2d.h"

enum TextStyleType
{
    Colored = 0,
    Delayed = 1,
    Instant = 2,
    Shake = 3
};

class TextStyleSection : public cocos2d::CCObject
{
public:
    static TextStyleSection* create(int start, int end, TextStyleType type);
    static TextStyleSection* createDelaySection(int start, float delay);
    static TextStyleSection* createInstantSection(int start, int end, float duration);
    static TextStyleSection* createShakeSection(int start, int end, int intensity, int perSecond);
    
    bool init(int start, int end, TextStyleType type);
    
    ~TextStyleSection();

public:
    TextStyleType m_styleType;
    int m_startIndex;
    int m_endIndex;
    float m_delay;
    float m_instantTime;
    int m_shakeIntensity;
    int m_shakesPerSecond;
    cocos2d::ccColor3B m_color;
};