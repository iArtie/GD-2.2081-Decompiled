#include "GauntletSprite.h"
#include "GauntletNode.h"
#include "CCSpriteGrayscale.h"
#include "CCSpriteWithHue.h"

GauntletSprite* GauntletSprite::create(GauntletType type, bool locked)
{
    GauntletSprite* instance = new GauntletSprite();
    memset(instance, 0, sizeof(GauntletSprite));
    if (instance && instance->init(type, locked))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GauntletSprite::init(GauntletType type, bool locked)
{
    if (!cocos2d::CCNode::init())
        return false;

    m_gauntletType = type;
    setContentSize(cocos2d::CCSize(60.0f, 60.0f));
    toggleLockedSprite(locked);

    return true;
}

void GauntletSprite::toggleLockedSprite(bool locked)
{
    removeAllChildren();
    if (locked)
        addLockedSprite();
    else
        addNormalSprite();
}

void GauntletSprite::addNormalSprite()
{
    std::string frame = GauntletNode::frameForType(m_gauntletType);
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(frame.c_str());
    addChild(sprite);

    cocos2d::CCSize size = getContentSize();
    sprite->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
}

void GauntletSprite::addLockedSprite()
{
    std::string frame = GauntletNode::frameForType(m_gauntletType);

    // Create grayscale version of the sprite
    CCSpriteGrayscale* grayscale = CCSpriteGrayscale::createWithSpriteFrameName(frame.c_str());
    grayscale->setColor(cocos2d::ccc3(0, 255, 255)); // Cyan tint for locked
    addChild(grayscale);

    // Set luminance
    float luminance = luminanceForType(m_gauntletType);
    grayscale->setLuminance(luminance);

    // Position
    cocos2d::CCSize size = getContentSize();
    grayscale->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));

    // Set color based on type
    cocos2d::ccColor3B color = colorForType(m_gauntletType);
    grayscale->setColor(color);

    // Add lock icon
    cocos2d::CCSprite* lock = cocos2d::CCSprite::createWithSpriteFrameName("gauntletLock_001.png");
    grayscale->addChild(lock, 1);

    cocos2d::CCSize graySize = grayscale->getContentSize();
    lock->setPosition(cocos2d::CCPoint(graySize.width * 0.5f, graySize.height * 0.5f));
}

cocos2d::ccColor3B GauntletSprite::colorForType(GauntletType type)
{
    // Default color
    return cocos2d::ccc3(148, 34, 34);
}

float GauntletSprite::luminanceForType(GauntletType type)
{
    // Luminance values for different gauntlet types
    // Values extracted from the decompiled switch
    static float luminanceValues[] = {
        1.0f,   // Fire
        0.0f,   // Ice
        0.0f,   // Poison
        0.0f,   // Shadow
        0.0f,   // Lava
        0.0f,   // Bonus
        0.0f,   // Chaos
        0.0f,   // Demon
        0.0f,   // Time
        0.0f,   // Crystal
        0.0f,   // Magic
        0.0f,   // Spike
        0.0f    // Monster
    };

    int index = type - 1;
    if (index >= 0 && index < 13)
        return luminanceValues[index];

    return 1.0f;
}

GauntletSprite::~GauntletSprite()
{
    // Destructor
}