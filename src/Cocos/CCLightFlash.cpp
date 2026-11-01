#include "Cocos/CCLightFlash.h"
#include "Platform/cocos2d.h"

void CCLightFlash::cleanupFlash() {
    if (m_layerColor) {
        m_layerColor->removeFromParentAndCleanup(true);
    }
    this->removeFromParentAndCleanup(true);
}

CCLightFlash* CCLightFlash::create() {
    CCLightFlash* flash = new CCLightFlash();
    if (flash && flash->init()) {
        flash->autorelease();
        return flash;
    }
    delete flash;
    return nullptr;
}

void CCLightFlash::fadeAndRemove() {
    float maxDuration = 0.0f;

    // Animate each light strip with random delays
    for (unsigned int i = 0; i < m_lightStripArray->count(); i++) {
        cocos2d::CCObject* obj = m_lightStripArray->objectAtIndex(i);

        float randomDelay = static_cast<float>(rand()) * 4.6566e-10f * 0.2f;
        float randomDuration = (static_cast<float>(rand()) * 4.6566e-10f * 2.0f - 1.0f) * 0.1f + 0.5f;

        cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(randomDelay);
        cocos2d::CCActionTween* tween = cocos2d::CCActionTween::create(
            "opacity",
            randomDuration,
            obj->getOpacity(),
            0.0f
        );
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(obj, callfunc_selector(cocos2d::CCNode::removeFromParentAndCleanup));

        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(delay, tween, callFunc, nullptr);
        obj->runAction(sequence);

        float duration = randomDelay + randomDuration;
        if (maxDuration < duration) {
            maxDuration = duration;
        }
    }

    // Schedule cleanup after all animations complete
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(maxDuration);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(CCLightFlash::cleanupFlash));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(delay, callFunc, nullptr);
    this->runAction(sequence);
}

bool CCLightFlash::init() {
    m_layerColor = nullptr;
    m_lightStripArray = cocos2d::CCArray::create();
    m_lightStripArray->retain();
    m_dontFadeOut = false;
    m_mainLayer = nullptr;
    m_layerColorZOrder = 0;
    return true;
}

void CCLightFlash::removeLights() {
    for (unsigned int i = 0; i < m_lightStripArray->count(); i++) {
        cocos2d::CCObject* obj = m_lightStripArray->objectAtIndex(i);
        obj->removeFromParentAndCleanup(true);
    }
    m_lightStripArray->removeAllObjects();
}

void CCLightFlash::showFlash() {
    if (m_dontFadeOut) {
        if (!m_mainLayer) {
            m_mainLayer = this;
        }

        m_layerColor = cocos2d::CCLayerColor::create();
        m_layerColor->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
        m_mainLayer->addChild(m_layerColor, m_layerColorZOrder);

        cocos2d::CCFadeIn* fadeIn = cocos2d::CCFadeIn::create(0.3f);
        cocos2d::CCEaseIn* easeIn = cocos2d::CCEaseIn::create(fadeIn, 2.0f);
        cocos2d::CCCallFunc* removeLightsCall = cocos2d::CCCallFunc::create(this, callfunc_selector(CCLightFlash::removeLights));
        cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.5f);
        cocos2d::CCFadeOut* fadeOut = cocos2d::CCFadeOut::create(1.5f);
        cocos2d::CCEaseInOut* easeOut = cocos2d::CCEaseInOut::create(fadeOut, 2.0f);
        cocos2d::CCCallFunc* cleanupCall = cocos2d::CCCallFunc::create(this, callfunc_selector(CCLightFlash::cleanupFlash));

        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
            easeIn,
            removeLightsCall,
            delay,
            easeOut,
            cleanupCall,
            nullptr
        );
        m_layerColor->runAction(sequence);
    } else {
        fadeAndRemove();
    }
}

CCLightFlash::~CCLightFlash() {
    CC_SAFE_RELEASE_NULL(m_lightStripArray);
    CC_SAFE_RELEASE_NULL(m_layerColor);
}