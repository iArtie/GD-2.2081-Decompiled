#include "UI/AnimatedShopKeeper.h"
#include "Cocos/CCAnimatedSprite.h"
#include "Audio/FMODAudioEngine.h"
#include "Platform/cocos2d.h"
#include <cstdlib>
#include <string>

void AnimatedShopKeeper::animationFinished(const char* key)
{
    m_idleInt2--;
    m_idleInt1--;
    
    std::string animKey(key);
    bool isReact01 = (animKey == "react01");
    bool isIdle03 = (animKey == "idle03");
    bool isIdle00 = (animKey == "idle00");
    
    if (isReact01)
    {
        m_reacting = false;
    }
    
    if (isIdle03)
    {
        if (m_idleInt2 <= 0 && m_type != Diamond)
        {
            m_looking = false;
            m_idleInt2 = (int)(((float)rand() / RAND_MAX) * 5.0f + 10.0f);
            m_animatedSprite->runAnimationForced("idle03");
            return;
        }
    }
    
    if (isIdle00)
    {
        if (m_idleInt1 <= 0)
        {
            if ((float)rand() / RAND_MAX <= 0.9f || m_type == Diamond)
            {
                m_animatedSprite->runAnimationForced("idle02");
            }
            else
            {
                m_animatedSprite->runAnimationForced("idle01");
            }
            m_idleInt1 = (int)(((float)rand() / RAND_MAX) * 3.0f + 2.0f);
            return;
        }
    }
    
    if (animKey == "idle03")
    {
        if (!m_looking && ((float)rand() / RAND_MAX) > 0.6f)
        {
            m_animatedSprite->runAnimationForced("idle03");
            m_looking = true;
        }
        else
        {
            m_animatedSprite->runAnimationForced("idle00");
        }
    }
    else
    {
        m_animatedSprite->runAnimationForced("idle00");
    }
}

AnimatedShopKeeper* AnimatedShopKeeper::create(ShopType type)
{
    AnimatedShopKeeper* pRet = new AnimatedShopKeeper();
    if (pRet && pRet->init(type))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool AnimatedShopKeeper::init(ShopType type)
{
    m_type = type;
    
    const char* typeName = nullptr;
    switch (type)
    {
        case Secret:
            typeName = "GJShopKeeper2";
            break;
        case Community:
            typeName = "GJShopKeeper3";
            break;
        case Mechanic:
            typeName = "GJShopKeeper4";
            break;
        case Diamond:
            typeName = "GJShopKeeper5";
            break;
        default:
            typeName = "GJShopKeeper";
            break;
    }
    
    if (!CCAnimatedSprite::initWithType(this, typeName, 0, 0))
    {
        return false;
    }
    
    m_idleInt2 = (int)(((float)rand() / RAND_MAX) * 5.0f + 10.0f);
    m_idleInt1 = (int)(((float)rand() / RAND_MAX) * 2.0f + 1.0f);
    
    SpriteAnimationManager::stopAnimations(m_animationManager);
    
    return true;
}

void AnimatedShopKeeper::playReactAnimation()
{
    if (m_reacting)
    {
        return;
    }
    
    m_gruntIndex++;
    if (m_gruntIndex > 3)
    {
        m_gruntIndex = 1;
    }
    
    std::string soundName = "grunt0" + std::to_string(m_gruntIndex) + ".ogg";
    FMODAudioEngine::sharedEngine()->playEffect(soundName, 1.0f, 0.0f, 0.65f);
    
    m_animatedSprite->runAnimationForced("react01");
    
    m_reacting = true;
    m_reactCount++;
}

void AnimatedShopKeeper::startAnimating()
{
    m_animatedSprite->runAnimationForced("idle00");
}

AnimatedShopKeeper::~AnimatedShopKeeper()
{
    // Destructor would call parent destructors
}