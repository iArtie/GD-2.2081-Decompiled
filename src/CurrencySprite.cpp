#include "CurrencySprite.h"

CurrencySprite* CurrencySprite::create(CurrencySpriteType type, bool burst)
{
    CurrencySprite* pRet = new CurrencySprite();
    if (pRet && pRet->init(type, burst))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

CurrencySprite* CurrencySprite::createWithSprite(cocos2d::CCSprite* sprite)
{
    CurrencySprite* pRet = new CurrencySprite();
    if (pRet && pRet->initWithSprite(sprite))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CurrencySprite::init(CurrencySpriteType type, bool burst)
{
    std::string spriteName;
    std::string glowName;
    cocos2d::ccColor3B mainColor = cocos2d::ccColor3B(255, 255, 255);
    cocos2d::ccColor3B glowColor1 = cocos2d::ccColor3B(255, 255, 255);
    cocos2d::ccColor3B glowColor2 = cocos2d::ccColor3B(255, 255, 255);
    float scale = 1.0f;

    switch (type)
    {
        case Star:
            spriteName = "GJ_bigStar_noShadow_001.png";
            glowName = "GJ_bigStar_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 150, 0);
            glowColor1 = cocos2d::ccColor3B(255, 150, 0);
            glowColor2 = cocos2d::ccColor3B(200, 255, 0);
            scale = 1.0f;
            break;
            
        case Diamond:
            spriteName = "GJ_bigDiamond_noShadow_001.png";
            glowName = "GJ_bigDiamond_glow_001.png";
            mainColor = cocos2d::ccColor3B(
                (unsigned char)(((float)rand() * 4.6566e-10f) * 125.0f + 130.0f),
                (unsigned char)(((float)rand() * 4.6566e-10f) * 125.0f + 130.0f),
                (unsigned char)(((float)rand() * 4.6566e-10f) * 175.0f + 80.0f)
            );
            glowColor1 = cocos2d::ccColor3B(255, 255, 255);
            glowColor2 = cocos2d::ccColor3B(255, 255, 255);
            scale = 1.0f;
            break;
            
        case FireShard:
            spriteName = "fireShardBig_001.png";
            glowName = "shard_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 150, 0);
            glowColor1 = cocos2d::ccColor3B(255, 255, 255);
            glowColor2 = cocos2d::ccColor3B(255, 100, 0);
            scale = 1.0f;
            break;
            
        case IceShard:
            spriteName = "iceShardBig_001.png";
            glowName = "shard_glow_001.png";
            mainColor = cocos2d::ccColor3B(200, 255, 255);
            glowColor1 = cocos2d::ccColor3B(0, 255, 255);
            glowColor2 = cocos2d::ccColor3B(0, 150, 255);
            scale = 1.0f;
            break;
            
        case PoisonShard:
            spriteName = "poisonShardBig_001.png";
            glowName = "shard_glow_001.png";
            mainColor = cocos2d::ccColor3B(150, 255, 0);
            glowColor1 = cocos2d::ccColor3B(150, 255, 0);
            glowColor2 = cocos2d::ccColor3B(100, 255, 0);
            scale = 1.0f;
            break;
            
        case ShadowShard:
            spriteName = "shadowShardBig_001.png";
            glowName = "shard_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 0, 255);
            glowColor1 = cocos2d::ccColor3B(100, 0, 255);
            glowColor2 = cocos2d::ccColor3B(255, 0, 255);
            scale = 1.0f;
            break;
            
        case LavaShard:
            spriteName = "lavaShardBig_001.png";
            glowName = "shard_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 150, 0);
            glowColor1 = cocos2d::ccColor3B(255, 50, 0);
            glowColor2 = cocos2d::ccColor3B(100, 255, 0);
            scale = 1.0f;
            break;
            
        case Key:
            spriteName = "GJ_bigKey_001.png";
            glowName = "GJ_bigKey_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 255, 255);
            glowColor1 = cocos2d::ccColor3B(255, 255, 255);
            glowColor2 = cocos2d::ccColor3B(255, 255, 255);
            scale = 1.5f;
            break;
            
        case Shard2_1:
            spriteName = "shard0201ShardBig_001.png";
            glowName = "shard2_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 150, 0);
            glowColor1 = cocos2d::ccColor3B(255, 200, 0);
            glowColor2 = cocos2d::ccColor3B(255, 150, 0);
            scale = 1.0f;
            break;
            
        case Shard2_2:
            spriteName = "shard0202ShardBig_001.png";
            glowName = "shard2_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 0, 0);
            glowColor1 = cocos2d::ccColor3B(255, 50, 0);
            glowColor2 = cocos2d::ccColor3B(255, 0, 0);
            scale = 1.0f;
            break;
            
        case Shard2_3:
            spriteName = "shard0203ShardBig_001.png";
            glowName = "shard2_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 255, 255);
            glowColor1 = cocos2d::ccColor3B(255, 255, 255);
            glowColor2 = cocos2d::ccColor3B(255, 255, 255);
            scale = 1.0f;
            break;
            
        case Shard2_4:
            spriteName = "shard0204ShardBig_001.png";
            glowName = "shard2_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 255, 0);
            glowColor1 = cocos2d::ccColor3B(255, 255, 50);
            glowColor2 = cocos2d::ccColor3B(255, 255, 0);
            scale = 1.0f;
            break;
            
        case Shard2_5:
            spriteName = "shard0205ShardBig_001.png";
            glowName = "shard2_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 150, 0);
            glowColor1 = cocos2d::ccColor3B(0, 255, 175);
            glowColor2 = cocos2d::ccColor3B(175, 0, 255);
            scale = 1.0f;
            break;
            
        case Moon:
            spriteName = "GJ_bigMoon_noShadow_001.png";
            glowName = "GJ_bigMoon_glow_001.png";
            mainColor = cocos2d::ccColor3B(0, 255, 255);
            glowColor1 = cocos2d::ccColor3B(0, 255, 255);
            glowColor2 = cocos2d::ccColor3B(0, 200, 255);
            scale = 1.0f;
            break;
            
        case GoldKey:
            spriteName = "GJ_bigGoldKey_001.png";
            glowName = "GJ_bigGoldKey_glow_001.png";
            mainColor = cocos2d::ccColor3B(255, 150, 50);
            glowColor1 = cocos2d::ccColor3B(255, 150, 50);
            glowColor2 = cocos2d::ccColor3B(200, 255, 50);
            scale = 1.5f;
            break;
            
        default: // Orb
            spriteName = "currencyOrb_001.png";
            scale = 1.0f;
            break;
    }

    if (!cocos2d::CCSprite::initWithSpriteFrameName(spriteName.c_str()))
        return false;

    // Create burst sprite
    if (burst && !glowName.empty())
    {
        m_burstSprite = cocos2d::CCSprite::createWithSpriteFrameName(glowName.c_str());
        if (m_burstSprite)
        {
            float baseScale = (type == Star) ? 0.63f : 0.6f;
            m_burstSprite->setScale(baseScale * scale);
            m_burstSprite->retain();
            m_burstSprite->setColor(mainColor);

            // Create shine burst
            cocos2d::CCSprite* shine1 = cocos2d::CCSprite::createWithSpriteFrameName("shineBurst_001.png");
            m_burstSprite->addChild(shine1);
            shine1->setPosition(cocos2d::CCPoint(0, 0));

            float direction = ((float)rand() * 4.6566e-10f) > 0.5f ? 1.0f : -1.0f;
            float rotation = ((float)rand() * 4.6566e-10f) * 0.5f + 0.5f;
            shine1->runAction(cocos2d::CCRepeatForever::create(
                cocos2d::CCRotateBy::create(1.0f, direction * rotation * 180.0f)
            ));
            shine1->setScale(((float)rand() * 4.6566e-10f) + 1.8f);
            shine1->setOpacity((unsigned char)((((float)rand() * 4.6566e-10f) * 0.4f + 0.6f) * 255.0f));
            shine1->setColor(glowColor1);

            // Second shine burst
            cocos2d::CCSprite* shine2 = cocos2d::CCSprite::createWithSpriteFrameName("shineBurst_001.png");
            shine1->addChild(shine2);
            shine2->setPosition(cocos2d::CCPoint(0, 0));
            shine2->setScale(0.8f);
            shine2->setColor(glowColor2);
            shine2->runAction(cocos2d::CCRepeatForever::create(
                cocos2d::CCRotateBy::create(1.0f, -direction * rotation * 180.0f)
            ));

            // Glow overlay
            cocos2d::CCSprite* glow = cocos2d::CCSprite::createWithSpriteFrameName(glowName.c_str());
            addChild(glow, 10);
            glow->setPosition(cocos2d::CCPoint(0, 0));
            glow->runAction(cocos2d::CCEaseIn::create(cocos2d::CCFadeOut::create(0.5f), 1.3f));
        }
    }

    m_spriteType = type;
    return true;
}

bool CurrencySprite::initWithSprite(cocos2d::CCSprite* sprite)
{
    if (!cocos2d::CCSprite::init())
        return false;

    addChild(sprite);
    sprite->setPosition(cocos2d::CCPoint(0, 0));

    // Create burst effect
    m_burstSprite = cocos2d::CCSprite::createWithSpriteFrameName("shineBurst_001.png");
    m_burstSprite->retain();

    float direction = ((float)rand() * 4.6566e-10f) > 0.5f ? -1.0f : 1.0f;
    float rotation = ((float)rand() * 4.6566e-10f) > 0.5f ? 90.0f : -90.0f;

    m_burstSprite->runAction(cocos2d::CCRepeatForever::create(
        cocos2d::CCRotateBy::create(1.0f, rotation)
    ));
    m_burstSprite->setScale(2.0f);
    m_burstSprite->setColor(cocos2d::ccColor3B(255, 255, 255));

    // Second shine
    cocos2d::CCSprite* shine2 = cocos2d::CCSprite::createWithSpriteFrameName("shineBurst_001.png");
    m_burstSprite->addChild(shine2);
    shine2->setPosition(cocos2d::CCPoint(0, 0));
    shine2->setScale(0.8f);
    shine2->setColor(cocos2d::ccColor3B(255, 255, 255));
    shine2->runAction(cocos2d::CCRepeatForever::create(
        cocos2d::CCRotateBy::create(1.0f, direction * 180.0f)
    ));

    return true;
}

CurrencySpriteType CurrencySprite::rewardToSpriteType(int rewardType)
{
    static CurrencySpriteType types[] = {
        Orb, Star, Diamond, FireShard, IceShard, PoisonShard,
        ShadowShard, LavaShard, Key, Shard2_1, Shard2_2,
        Shard2_3, Shard2_4, Shard2_5, Moon, GoldKey
    };
    
    int index = rewardType - 1;
    if (index >= 0 && index < 15)
        return types[index];
    return Orb;
}

std::string CurrencySprite::spriteTypeToStat(CurrencySpriteType type)
{
    switch (type)
    {
        case FireShard:   return "18";
        case IceShard:    return "19";
        case PoisonShard: return "17";
        case ShadowShard: return "16";
        case LavaShard:   return "20";
        case Key:         return "21";
        case Shard2_1:    return "23";
        case Shard2_2:    return "24";
        case Shard2_3:    return "25";
        case Shard2_4:    return "26";
        case Shard2_5:    return "27";
        case GoldKey:     return "43";
        default:          return "";
    }
}

CurrencySprite::~CurrencySprite()
{
    CC_SAFE_RELEASE_NULL(m_burstSprite);
}