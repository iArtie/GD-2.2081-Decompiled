#include "Utils/TextStyleSection.h"

TextStyleSection* TextStyleSection::create(int start, int end, TextStyleType type)
{
    TextStyleSection* section = new TextStyleSection();
    if (section && section->init(start, end, type))
    {
        section->autorelease();
        return section;
    }
    
    delete section;
    return nullptr;
}

TextStyleSection* TextStyleSection::createDelaySection(int start, float delay)
{
    TextStyleSection* section = TextStyleSection::create(start, -1, Delayed);
    if (section)
    {
        section->m_delay = delay;
    }
    return section;
}

TextStyleSection* TextStyleSection::createInstantSection(int start, int end, float duration)
{
    TextStyleSection* section = TextStyleSection::create(start, end, Instant);
    if (section)
    {
        section->m_instantTime = duration;
    }
    return section;
}

TextStyleSection* TextStyleSection::createShakeSection(int start, int end, int intensity, int perSecond)
{
    TextStyleSection* section = TextStyleSection::create(start, end, Shake);
    if (section)
    {
        section->m_shakeIntensity = intensity;
        section->m_shakesPerSecond = perSecond;
    }
    return section;
}

bool TextStyleSection::init(int start, int end, TextStyleType type)
{
    m_styleType = type;
    m_startIndex = start;
    m_endIndex = end;
    m_delay = 0.0f;
    m_instantTime = 0.0f;
    m_shakeIntensity = 0;
    m_shakesPerSecond = 0;
    m_color = cocos2d::ccc3(255, 255, 255);
    return true;
}

TextStyleSection::~TextStyleSection()
{
}