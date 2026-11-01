#pragma once

#include "Platform/cocos2d.h"
#include <string>

enum MenuAnimationType
{
    Scale = 0,
    Move = 1
};

class CCMenuItemSpriteExtra : public cocos2d::CCMenuItemSprite
{
public:
    static CCMenuItemSpriteExtra* create(cocos2d::CCNode* normalSprite, cocos2d::CCNode* selectedSprite, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    
    void activate();
    void selected();
    void unselected();
    
    void setSizeMult(float mult);
    void useAnimationType(MenuAnimationType type);
    void setBaseScale(float scale);
    void setOffset(const cocos2d::CCPoint& offset);
    void setAnimationEnabled(bool enabled);
    void setColorEnabled(bool enabled);
    void setSelectSound(const std::string& sound, float volume);
    void setActivateSound(const std::string& sound, float volume);
    
    virtual ~CCMenuItemSpriteExtra();

private:
    bool m_bEnabled;
    bool m_colorEnabled;
    bool m_animationEnabled;
    MenuAnimationType m_animationType;
    float m_baseScale;
    float m_scaleMultiplier;
    float m_duration;
    float m_unselectedDuration;
    float m_colorDip;
    float m_volume;
    float m_fSizeMult;
    cocos2d::CCPoint m_startPosition;
    cocos2d::CCPoint m_offset;
    std::string m_activateSound;
    std::string m_selectSound;
};