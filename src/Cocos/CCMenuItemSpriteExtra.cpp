#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Audio/FMODAudioEngine.h"

CCMenuItemSpriteExtra* CCMenuItemSpriteExtra::create(cocos2d::CCNode* normalSprite, cocos2d::CCNode* selectedSprite, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector)
{
    CCMenuItemSpriteExtra* item = new CCMenuItemSpriteExtra();
    if (item && item->initWithNormalSprite(normalSprite, selectedSprite, nullptr, target, selector))
    {
        item->autorelease();
        item->m_bEnabled = true;
        item->m_colorEnabled = true;
        item->m_animationEnabled = true;
        item->m_animationType = Scale;
        item->m_baseScale = 1.0f;
        item->m_scaleMultiplier = 1.2f;
        item->m_duration = 0.2f;
        item->m_unselectedDuration = 0.2f;
        item->m_colorDip = 0.5f;
        item->m_volume = 1.0f;
        item->m_fSizeMult = 1.0f;
        item->m_offset = cocos2d::CCPoint(0.0f, 0.0f);
        return item;
    }
    
    delete item;
    return nullptr;
}

void CCMenuItemSpriteExtra::activate()
{
    if (!m_bEnabled)
    {
        return;
    }
    
    stopAllActions();
    
    if (m_animationType == Scale)
    {
        setScale(m_baseScale);
    }
    
    if (!m_activateSound.empty())
    {
        FMODAudioEngine::sharedEngine()->playEffect(m_activateSound, 1.0f, 0.0f, m_volume);
    }
    
    cocos2d::CCMenuItem::activate();
}

void CCMenuItemSpriteExtra::selected()
{
    if (!m_bEnabled)
    {
        return;
    }
    
    cocos2d::CCMenuItemSprite::selected();
    
    if (!m_selectSound.empty())
    {
        FMODAudioEngine::sharedEngine()->playEffect(m_selectSound, 1.0f, 0.0f, m_volume);
    }
    
    if (m_colorEnabled)
    {
        cocos2d::CCNode* normalImage = getNormalImage();
        if (normalImage)
        {
            unsigned char color = (unsigned char)(m_colorDip * 255.0f);
            normalImage->setColor(cocos2d::ccc3(color, color, color));
        }
    }
    
    if (m_animationEnabled)
    {
        if (m_animationType == Scale)
        {
            stopActionByTag(0);
            cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(m_duration, m_baseScale * m_scaleMultiplier);
            cocos2d::CCEaseBounceOut* ease = cocos2d::CCEaseBounceOut::create(scaleTo);
            ease->setTag(0);
            runAction(ease);
        }
        else if (m_animationType == Move)
        {
            cocos2d::CCNode* normalImage = getNormalImage();
            if (normalImage)
            {
                normalImage->stopActionByTag(0);
                cocos2d::CCPoint targetPos = m_startPosition + m_offset;
                cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(m_duration, targetPos);
                cocos2d::CCEaseInOut* ease = cocos2d::CCEaseInOut::create(moveTo, 1.5f);
                ease->setTag(0);
                normalImage->runAction(ease);
            }
        }
    }
}

void CCMenuItemSpriteExtra::unselected()
{
    if (!m_bEnabled)
    {
        return;
    }
    
    cocos2d::CCMenuItemSprite::unselected();
    
    if (m_colorEnabled)
    {
        cocos2d::CCNode* normalImage = getNormalImage();
        if (normalImage)
        {
            normalImage->setColor(cocos2d::ccc3(255, 255, 255));
        }
    }
    
    if (m_animationEnabled)
    {
        if (m_animationType == Scale)
        {
            stopActionByTag(0);
            cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(m_unselectedDuration, m_baseScale);
            cocos2d::CCEaseBounceOut* ease = cocos2d::CCEaseBounceOut::create(scaleTo);
            ease->setTag(0);
            runAction(ease);
        }
        else if (m_animationType == Move)
        {
            cocos2d::CCNode* normalImage = getNormalImage();
            if (normalImage)
            {
                normalImage->stopActionByTag(0);
                cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(m_unselectedDuration, m_startPosition);
                cocos2d::CCEaseInOut* ease = cocos2d::CCEaseInOut::create(moveTo, 2.0f);
                ease->setTag(0);
                normalImage->runAction(ease);
            }
        }
    }
}

void CCMenuItemSpriteExtra::setSizeMult(float mult)
{
    m_fSizeMult = mult;
}

void CCMenuItemSpriteExtra::useAnimationType(MenuAnimationType type)
{
    cocos2d::CCNode* normalImage = getNormalImage();
    if (normalImage)
    {
        m_startPosition = normalImage->getPosition();
    }
    m_animationType = type;
}

void CCMenuItemSpriteExtra::setBaseScale(float scale)
{
    m_baseScale = scale;
}

void CCMenuItemSpriteExtra::setOffset(const cocos2d::CCPoint& offset)
{
    m_offset = offset;
}

void CCMenuItemSpriteExtra::setAnimationEnabled(bool enabled)
{
    m_animationEnabled = enabled;
}

void CCMenuItemSpriteExtra::setColorEnabled(bool enabled)
{
    m_colorEnabled = enabled;
}

void CCMenuItemSpriteExtra::setSelectSound(const std::string& sound, float volume)
{
    m_selectSound = sound;
    m_volume = volume;
}

void CCMenuItemSpriteExtra::setActivateSound(const std::string& sound, float volume)
{
    m_activateSound = sound;
    m_volume = volume;
}

CCMenuItemSpriteExtra::~CCMenuItemSpriteExtra()
{
}