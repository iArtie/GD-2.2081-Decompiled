#include "Objects/AnimatedGameObject.h"
#include "Cocos/CCAnimatedSprite.h"
#include "Cocos/CCSpritePart.h"
#include "Objects/GameObject.h"
#include "Managers/GameManager.h"
#include <cstdlib>

void AnimatedGameObject::activateObject()
{
    GameObject::activateObject(this);
    if (m_isActivated)
    {
        updateObjectAnimation();
    }
}

void AnimatedGameObject::animationFinished(const char* key)
{
    if (m_finishedAnimating)
    {
        return;
    }

    if (!m_animatedSprite)
    {
        if (m_objectID != 918 && m_objectID != 1327 && m_objectID != 1328 && m_objectID != 1584 && m_objectID != 2012)
        {
            m_animationID = 0;
            return;
        }
        return;
    }

    // Complex animation state machine - simplified representation
    switch (m_objectID)
    {
        case 918:
            handleBeast01Animation(key);
            break;
        case 1327:
            handleBeast02Animation(key);
            break;
        case 1328:
            handleBeast03Animation(key);
            break;
        case 1584:
            handleBeast04Animation(key);
            break;
        case 2012:
            handleBeast05Animation(key);
            break;
        default:
            break;
    }
}

void AnimatedGameObject::handleBeast01Animation(const char* key)
{
    // GJBeast01 (object 918) animation transitions
    if (std::string(key) == "attack01")
    {
        m_animatedSprite->runAnimationForced("attack01_loop");
        m_animationID = 1;
    }
    else
    {
        m_animatedSprite->runAnimationForced("bite");
        m_animationID = 0;
    }
}

void AnimatedGameObject::handleBeast02Animation(const char* key)
{
    // GJBeast02 (object 1327) animation transitions
    if (std::string(key) == "idle01")
    {
        float randVal = (float)rand() / RAND_MAX;
        if (randVal <= 0.75f)
        {
            m_animatedSprite->runAnimationForced("idle01");
        }
        else
        {
            m_animatedSprite->runAnimationForced("idle02");
        }
        m_animationID = 0;
    }
}

void AnimatedGameObject::handleBeast03Animation(const char* key)
{
    // GJBeast03 (object 1328) animation transitions
    if (std::string(key) == "idle01")
    {
        float randVal = (float)rand() / RAND_MAX;
        if (randVal <= 0.9f)
        {
            m_animatedSprite->runAnimationForced("idle01");
        }
        else
        {
            m_animatedSprite->runAnimationForced("idle02");
        }
        m_animationID = 0;
    }
}

void AnimatedGameObject::handleBeast04Animation(const char* key)
{
    // GJBeast04 (object 1584) animation transitions
    std::string animKey(key);
    
    if (animKey == "attack02")
    {
        m_animatedSprite->runAnimationForced("attack02_loop");
        m_animationID = 9;
    }
    else if (animKey == "sleep")
    {
        m_animatedSprite->runAnimationForced("sleep_loop");
        m_animationID = 7;
    }
    else if (animKey == "idle01")
    {
        float randVal = (float)rand() / RAND_MAX;
        if (randVal > 0.8f)
        {
            m_animatedSprite->runAnimationForced("idle02");
        }
        else
        {
            m_animatedSprite->runAnimationForced("idle01");
        }
        m_animationID = 0;
    }
}

void AnimatedGameObject::handleBeast05Animation(const char* key)
{
    // GJBeast05 (object 2012) animation transitions
    std::string animKey(key);
    
    if (animKey == "toAttack03")
    {
        m_animatedSprite->runAnimationForced("attack03");
        m_animationID = 6;
    }
    else if (animKey == "attack03")
    {
        m_animationID = 0;
    }
    else if (animKey == "attack01")
    {
        m_animationID = 3;
    }
    else if (animKey == "attack02")
    {
        m_animationID = 4;
    }
    else if (animKey == "fromAttack03")
    {
        if (std::string(key) == "fromAttack03")
        {
            m_animatedSprite->runAnimationForced("attack01");
            m_animationID = 3;
        }
    }
}

std::string AnimatedGameObject::animationForID(int type, int id)
{
    switch (id)
    {
        case 918: // GJBeast01
            switch (type)
            {
                case 0: return "bite";
                case 1: return "attack01";
                case 2: return "attack01_end";
                case 3: return "idle01";
                default: return "";
            }
        case 1584: // GJBeast04
            switch (type)
            {
                case 0: return "idle01";
                case 1: return "idle02";
                case 2: return "idle03";
                case 3: return "attack01";
                case 4: return "attack02";
                case 5: return "attack02_end";
                case 6: return "sleep";
                case 7: return "sleep_loop";
                case 8: return "sleep_end";
                case 9: return "attack02_loop";
                default: return "";
            }
        case 2012: // GJBeast05
            switch (type)
            {
                case 0: return "idle01";
                case 1: return "idle02";
                case 2: return "toAttack01";
                case 3: return "attack01";
                case 4: return "attack02";
                case 5: return "toAttack03";
                case 6: return "attack03";
                case 7: return "idle03";
                case 8: return "fromAttack03";
                default: return "";
            }
        case 1327: // GJBeast02
        case 1328: // GJBeast03
        default:
            return "";
    }
}

AnimatedGameObject* AnimatedGameObject::create(int id)
{
    AnimatedGameObject* pRet = new AnimatedGameObject();
    if (pRet && pRet->init(id))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void AnimatedGameObject::deactivateObject(bool deactivate)
{
    EnhancedGameObject::deactivateObject(this, deactivate);
    if (m_isActivated)
    {
        SpriteAnimationManager::stopAnimations(m_animatedSprite->m_animationManager);
    }
}

float AnimatedGameObject::getTweenTime(int type, int id)
{
    return 0.05f;
}

bool AnimatedGameObject::init(int id)
{
    m_objectID = id;
    m_classType = 0; // Animated

    const char* typeName = nullptr;
    switch (id)
    {
        case 918:
            typeName = "GJBeast01";
            m_notGrounded = true;
            break;
        case 919:
            typeName = "BlackSludge";
            break;
        case 1327:
            typeName = "GJBeast02";
            break;
        case 1328:
            typeName = "GJBeast03";
            break;
        case 1584:
            typeName = "GJBeast04";
            m_notGrounded = true;
            break;
        case 2012:
            typeName = "GJBeast05";
            m_notGrounded = true;
            break;
        default:
            return false;
    }

    m_animatedSprite = CCAnimatedSprite::createWithType(typeName, 0, 0);
    SpriteAnimationManager::stopAnimations(m_animatedSprite->m_animationManager);

    if (!GameObject::initWithTexture(this, m_animatedSprite->getTexture()))
    {
        return false;
    }

    setupAnimatedSize(id);
    addChild(this, m_animatedSprite);
    
    cocos2d::CCSize size = getContentSize(this);
    m_animatedSprite->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
    
    m_bUnkBool2 = false;
    m_unk367 = true;
    setupChildSprites();
    
    return true;
}

void AnimatedGameObject::playAnimation(int type)
{
    std::string anim = animationForID(type, m_objectID);
    
    if (!anim.empty())
    {
        m_animationID = type;
        m_playingAnimation = true;
        
        float tweenTime = getTweenTime(type, m_objectID);
        if (m_isActivated && tweenTime > 0.0f)
        {
            m_animatedSprite->tweenToAnimation(anim, tweenTime);
        }
        else
        {
            m_animatedSprite->runAnimationForced(anim);
        }
    }
}

void AnimatedGameObject::resetObject()
{
    m_animationID = 0;
    m_finishedAnimating = false;
    m_playingAnimation = false;
    m_currentAnimation = "";
    
    if (m_isActivated)
    {
        updateObjectAnimation();
    }
    EnhancedGameObject::resetObject(this);
}

void AnimatedGameObject::setChildColor(const cocos2d::ccColor3B* color)
{
    GameObject::setChildColor(this, color);
    if (m_eyeSpritePart)
    {
        m_eyeSpritePart->setColor(color);
        m_eyeSpritePart->setChildColor(color);
    }
}

void AnimatedGameObject::setObjectColor(const cocos2d::ccColor3B* color)
{
    cocos2d::CCSprite::setColor(this, color);
    m_animatedSprite->setColor(color);
}

void AnimatedGameObject::setOpacity(unsigned char opacity)
{
    GameObject::setOpacity(this, opacity);
    
    unsigned char finalOpacity = opacity;
    m_animatedSprite->setOpacity(opacity);
    
    if (m_childSprite)
    {
        if (m_hasNoGlow)
        {
            finalOpacity = 0;
        }
        m_childSprite->setOpacity(finalOpacity);
        
        cocos2d::CCArray* children = m_childSprite->getChildren();
        for (unsigned int i = 0; i < children->count(); i++)
        {
            cocos2d::CCObject* obj = children->objectAtIndex(i);
            obj->setOpacity(finalOpacity);
        }
    }
}

void AnimatedGameObject::setupAnimatedSize(int id)
{
    float width = 10.0f;
    float height = 10.0f;
    float radius = 10.0f;
    
    switch (id)
    {
        case 918:
            width = 48.0f;
            height = 48.0f;
            radius = 24.0f;
            m_hasContentSize = true;
            m_lastSize = cocos2d::CCSize(80.0f, 80.0f);
            break;
        case 919:
            width = 6.0f;
            height = 25.0f;
            break;
        case 1327:
            width = 8.0f;
            height = 8.0f;
            m_hasContentSize = true;
            m_lastSize = cocos2d::CCSize(35.0f, 30.0f);
            radius = 15.0f;
            break;
        case 1328:
            width = 8.0f;
            height = 15.0f;
            m_hasContentSize = true;
            m_lastSize = cocos2d::CCSize(35.0f, 40.0f);
            break;
        case 1584:
            width = 8.0f;
            height = 8.0f;
            m_hasContentSize = true;
            m_lastSize = cocos2d::CCSize(60.0f, 80.0f);
            break;
        case 2012:
            width = 8.0f;
            height = 8.0f;
            m_hasContentSize = true;
            m_lastSize = cocos2d::CCSize(45.0f, 45.0f);
            radius = 15.0f;
            break;
    }
    
    m_width = m_scaleX * width;
    m_height = m_scaleY * height;
    m_objectRadius = radius;
    setContentSize(cocos2d::CCSize(width, height));
}

void AnimatedGameObject::setupChildSprites()
{
    // This method sets up glow sprites and color sprites for each animated object
    // Implementation varies significantly per object ID
    
    switch (m_objectID)
    {
        case 918:
            setupBeast01ChildSprites();
            break;
        case 1327:
        case 1328:
            setupBeast02Or03ChildSprites();
            break;
        case 1584:
            setupBeast04ChildSprites();
            break;
        case 2012:
            setupBeast05ChildSprites();
            break;
    }
}

void AnimatedGameObject::setupBeast01ChildSprites()
{
    m_childSprite = cocos2d::CCSprite::create();
    m_animatedSprite->addChild(m_childSprite, -1);
    
    cocos2d::CCArray* parts = m_animatedSprite->m_paSprite->m_spriteParts;
    for (unsigned int i = 0; i < parts->count(); i++)
    {
        CCSpritePlus* part = (CCSpritePlus*)parts->objectAtIndex(i);
        cocos2d::CCSprite* glow;
        
        if (i == 1)
        {
            glow = cocos2d::CCSprite::createWithSpriteFrameName("GJBeast01_01_glow_001.png");
        }
        else
        {
            glow = cocos2d::CCSprite::createWithSpriteFrameName("GJBeast01_02_glow_001.png");
        }
        
        m_childSprite->addChild(glow, -1);
        CCSpritePlus::addFollower(part, glow);
        glow->setRotation(part->getRotation());
        glow->setScaleX(part->getScaleX());
        glow->setScaleY(part->getScaleY());
        glow->setPosition(part->getPosition());
        
        if (i == 1)
        {
            m_unk28c = true;
            m_colorSpriteLocked = true;
            GameObject::addCustomColorChild(this, "GJBeast01_03_001.png");
            GameObject::setDefaultSecondaryColorMode(this, 1011);
            part->addChild(m_colorSprite, 1);
            
            cocos2d::CCSize size = part->getContentSize();
            m_colorSprite->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
        }
    }
}

void AnimatedGameObject::setupBeast02Or03ChildSprites()
{
    m_childSprite = cocos2d::CCSprite::create();
    m_animatedSprite->addChild(m_childSprite, -1);
    
    cocos2d::CCArray* parts = m_animatedSprite->m_paSprite->m_spriteParts;
    if (parts->count() <= 1)
        return;
    
    CCSpritePlus* part = (CCSpritePlus*)parts->objectAtIndex(1);
    const char* glowName = (m_objectID == 1327) ? "GJBeast02_01_glow_001.png" : "GJBeast03_01_glow_001.png";
    
    cocos2d::CCSprite* glow = cocos2d::CCSprite::createWithSpriteFrameName(glowName);
    m_childSprite->addChild(glow, -1);
    CCSpritePlus::addFollower(part, glow);
    
    part->m_propagateScaleChanges = true;
    glow->setRotation(part->getRotation());
    glow->setScaleX(part->getScaleX());
    glow->setScaleY(part->getScaleY());
    glow->setPosition(part->getPosition());
    
    m_unk28c = true;
    m_colorSpriteLocked = true;
    m_colorSprite = (cocos2d::CCSprite*)parts->objectAtIndex(0);
    cocos2d::CCObject::retain(m_colorSprite);
    
    GameObject::createSpriteColor(this, 2);
    GameObject::setDefaultSecondaryColorMode(this, 1011);
}

void AnimatedGameObject::setupBeast04ChildSprites()
{
    m_childSprite = cocos2d::CCSprite::create();
    m_animatedSprite->addChild(m_childSprite, -1);
    
    cocos2d::CCArray* parts = m_animatedSprite->m_paSprite->m_spriteParts;
    m_unk28c = true;
    m_colorSpriteLocked = true;
    
    m_colorSprite = (cocos2d::CCSprite*)parts->objectAtIndex(0);
    cocos2d::CCObject::retain(m_colorSprite);
    
    GameObject::createSpriteColor(this, 2);
    GameObject::setDefaultSecondaryColorMode(this, 1011);
    
    for (unsigned int i = 1; i < parts->count(); i++)
    {
        cocos2d::CCObject* obj = parts->objectAtIndex(i);
        cocos2d::CCSprite* glow = nullptr;
        
        switch (i)
        {
            case 1: glow = cocos2d::CCSprite::createWithSpriteFrameName("GJBeast04_04_glow_001.png"); break;
            case 2: glow = cocos2d::CCSprite::createWithSpriteFrameName("GJBeast04_02_glow_001.png"); break;
            case 3: glow = cocos2d::CCSprite::createWithSpriteFrameName("GJBeast04_01_glow_001.png"); break;
        }
        
        if (glow)
        {
            m_childSprite->addChild(glow, -1);
            CCSpritePlus::addFollower((CCSpritePlus*)obj, glow);
            glow->setRotation(((CCSpritePlus*)obj)->getRotation());
            glow->setScaleX(((CCSpritePlus*)obj)->getScaleX());
            glow->setScaleY(((CCSpritePlus*)obj)->getScaleY());
            glow->setPosition(((CCSpritePlus*)obj)->getPosition());
            
            if (i == 1)
            {
                // Set sprite part delegate
            }
        }
    }
}

void AnimatedGameObject::setupBeast05ChildSprites()
{
    m_childSprite = cocos2d::CCSprite::create();
    m_animatedSprite->addChild(m_childSprite, -1);
    
    cocos2d::CCArray* parts = m_animatedSprite->m_paSprite->m_spriteParts;
    if (parts->count() <= 1)
        return;
    
    cocos2d::CCObject* obj = parts->objectAtIndex(0);
    cocos2d::CCSprite* glow = cocos2d::CCSprite::createWithSpriteFrameName("GJBeast05_02_glow_001.png");
    m_childSprite->addChild(glow, -1);
    CCSpritePlus::addFollower((CCSpritePlus*)obj, glow);
    
    glow->setRotation(((CCSpritePlus*)obj)->getRotation());
    glow->setScaleX(((CCSpritePlus*)obj)->getScaleX());
    glow->setScaleY(((CCSpritePlus*)obj)->getScaleY());
    glow->setPosition(((CCSpritePlus*)obj)->getPosition());
    
    m_unk28c = true;
    m_colorSpriteLocked = true;
    
    m_colorSprite = (cocos2d::CCSprite*)parts->objectAtIndex(1);
    cocos2d::CCObject::retain(m_colorSprite);
    
    m_eyeSpritePart = (CCSpritePart*)parts->objectAtIndex(2);
    cocos2d::CCObject::retain(m_eyeSpritePart);
    
    GameObject::createSpriteColor(this, 2);
    GameObject::setDefaultSecondaryColorMode(this, 1011);
}

void AnimatedGameObject::updateObjectAnimation()
{
    SpriteAnimationManager::stopAnimations(m_animatedSprite->m_animationManager);
    
    if (!m_isActivated)
        return;
    
    if (m_editorEnabled && !GameManager::sharedState()->m_levelEditorLayer->m_previewAnimations)
        return;
    
    if (m_playingAnimation && !m_currentAnimation.empty())
    {
        m_animatedSprite->m_animationManager->runAnimation(m_currentAnimation);
    }
    else
    {
        m_animatedSprite->m_animationManager->runAnimation(m_animatedSprite->m_currentAnim);
        m_animatedSprite->m_animationManager->offsetCurrentAnimation((float)rand() / RAND_MAX);
    }
}

AnimatedGameObject::~AnimatedGameObject()
{
    // Destructor would call parent destructors
}