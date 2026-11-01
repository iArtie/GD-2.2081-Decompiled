#include "TextArea.h"
#include "MultilineBitmapFont.h"

void TextArea::draw()
{
}

float TextArea::fadeIn(float duration, bool unused)
{
    stopAllActions();
    stopAllCharacterActions();
    setOpacity(0);
    m_allShown = false;
    
    cocos2d::CCFadeIn* fadeIn = cocos2d::CCFadeIn::create(duration);
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(duration);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(TextArea::finishFade));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(fadeIn, delay, callFunc, nullptr);
    runAction(sequence);
    
    return duration;
}

void TextArea::fadeOut(float duration)
{
    stopAllActions();
    stopAllCharacterActions();
    
    unsigned char opacity = getOpacity();
    cocos2d::CCActionTween* tween = cocos2d::CCActionTween::create("opacity", duration, (float)opacity, 0.0f);
    runAction(tween);
}

void TextArea::fadeOutAndRemove()
{
    fadeOut(0.3f);
    
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.3f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(TextArea::removeFromParentAndCleanup));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(delay, callFunc, nullptr);
    runAction(sequence);
}

void TextArea::finishFade()
{
    m_allShown = true;
    if (m_delegate)
    {
        m_delegate->fadeInTextFinished(this);
    }
}

void TextArea::hideAll()
{
    stopAllCharacterActions();
    setOpacity(0);
}

void TextArea::setIgnoreColorCode(bool ignore)
{
    m_disableColor = ignore;
    if (m_label)
    {
        m_label->m_disableColor = ignore;
    }
}

void TextArea::setOpacity(unsigned char opacity)
{
    cocos2d::CCSprite::setOpacity(opacity);
    if (m_label)
    {
        m_label->setOpacity(opacity);
    }
}

void TextArea::setString(const std::string& text)
{
    if (m_label)
    {
        hideAll();
        m_label->removeAllChildrenWithCleanup(true);
        m_label->removeFromParentAndCleanup(true);
        m_label = nullptr;
    }
    
    m_label = MultilineBitmapFont::createWithFont(
        m_fontFile.c_str(),
        text,
        m_scale,
        m_width,
        cocos2d::CCPoint(m_height, 0.0f),
        m_anchorPoint,
        0
    );
    
    m_rectWidth = m_label->m_width;
    m_rectHeight = m_label->m_height;
    m_unkPoint = m_label->m_position;
    m_maxWidth = m_label->m_maxWidth;
    
    cocos2d::CCRect rect(0.0f, 0.0f, m_rectWidth, m_rectHeight);
    setTextureRect(rect);
    addChild(m_label, 1);
    
    cocos2d::CCPoint pos(roundf(m_rectWidth * m_anchorPoint.x), roundf(m_rectHeight * m_anchorPoint.y));
    m_label->setPosition(pos);
}

void TextArea::showAll()
{
    stopAllCharacterActions();
    setOpacity(255);
    m_allShown = true;
}

void TextArea::stopAllCharacterActions()
{
    if (m_label && m_label->getChildren())
    {
        cocos2d::ccArray* children = m_label->getChildren()->data;
        for (unsigned int i = 0; i < children->num; i++)
        {
            cocos2d::CCNode* node = (cocos2d::CCNode*)children->arr[i];
            if (node)
            {
                node->stopAllActions();
            }
        }
    }
}

void TextArea::update(float dt)
{
    m_shakeElapsed += dt;
    
    if (m_shakeCharacters)
    {
        cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)m_shakeCharacters->m_pElements;
        while (element)
        {
            cocos2d::CCArray* array = (cocos2d::CCArray*)element->m_pObject;
            float* data = (float*)array->objectAtIndex(0);
            
            if (data && m_shakeElapsed > data[156])
            {
                data[156] = m_shakeElapsed + data[154];
                float xOffset = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * data[155] * 0.5f;
                float yOffset = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * data[155] * 0.5f;
                
                cocos2d::CCPoint offset(xOffset, yOffset);
                
                if (array)
                {
                    for (unsigned int i = 0; i < array->data->num; i++)
                    {
                        cocos2d::CCNode* node = (cocos2d::CCNode*)array->data->arr[i];
                        if (node)
                        {
                            cocos2d::CCPoint pos = node->getPosition() + offset;
                            node->setPosition(pos);
                        }
                    }
                }
            }
            
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }
}

TextArea::~TextArea()
{
}