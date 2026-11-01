#include "Objects/GhostTrailEffect.h"

GhostTrailEffect* GhostTrailEffect::create()
{
    GhostTrailEffect* instance = new GhostTrailEffect();
    if (instance && instance->init())
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GhostTrailEffect::init()
{
    if (!cocos2d::CCNode::init())
        return false;

    setVisible(false);
    m_color = cocos2d::ccc3(255, 255, 255);
    m_blendFunc.src = GL_ONE;
    m_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
    m_opacity = 255.0f;
    m_playerScale = 1.0f;
    m_delegate = nullptr;

    return true;
}

void GhostTrailEffect::draw()
{
    // Empty implementation - overridden by subclasses
}

void GhostTrailEffect::doBlendAdditive()
{
    m_blendFunc.src = GL_SRC_ALPHA;
    m_blendFunc.dst = GL_ONE;
}

void GhostTrailEffect::stopTrail()
{
    if (m_delegate)
    {
        // Call delegate method
    }
    unscheduleAllSelectors();
    stopAllActions();
    removeMeAndCleanup();
}

GhostTrailEffect::~GhostTrailEffect()
{
    // Destructor
}