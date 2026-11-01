#include "CCCountdown.h"
#include "cocos2d.h"

CCCountdown* CCCountdown::create() {
    CCCountdown* countdown = new CCCountdown();
    if (countdown && countdown->init()) {
        countdown->autorelease();
        return countdown;
    }
    delete countdown;
    return nullptr;
}

bool CCCountdown::init() {
    if (!cocos2d::CCSprite::initWithFile("timer_outerCircle.png")) {
        return false;
    }

    // Inner circle progress timer
    cocos2d::CCSprite* innerSprite = cocos2d::CCSprite::create("timer_innerCircle.png");
    m_progressTimer = cocos2d::CCProgressTimer::create(innerSprite);
    m_progressTimer->setType(cocos2d::kCCProgressTimerTypeRadial);
    this->addChild(m_progressTimer, -1);

    cocos2d::CCSize size = this->getContentSize();
    m_progressTimer->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
    m_progressTimer->setPercentage(100.0f);

    // Set inner circle color
    m_progressTimer->m_pSprite->setColor(cocos2d::ccc3(40, 40, 70));
    m_progressTimer->updateColor();
    m_progressTimer->m_bReverseDirection = true;

    // Countdown label
    m_countdownLabel = cocos2d::CCLabelBMFont::create();
    m_countdownLabel->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
    m_countdownLabel->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    
    cocos2d::CCPoint labelPos = m_countdownLabel->getPosition();
    labelPos.y -= 3.0f;
    m_countdownLabel->setPosition(labelPos);
    
    this->addChild(m_countdownLabel, 1);

    return true;
}

void CCCountdown::lapFinished() {
    m_seconds--;

    if (m_seconds <= 0) {
        if (m_targetNode && m_callbackSelector.ptr) {
            (m_targetNode->*m_callbackSelector)();
        }
        this->removeFromParentAndCleanup(true);
        return;
    }

    CCString* str = CCString::createWithFormat("%i", m_seconds);
    m_countdownLabel->setString(str->getCString());
    m_progressTimer->setPercentage(100.0f);

    cocos2d::CCProgressFromTo* progress = cocos2d::CCProgressFromTo::create(1.0f, 100.0f, 0.0f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(CCCountdown::lapFinished));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(progress, callFunc, nullptr);
    sequence->setTag(0);
    m_progressTimer->runAction(sequence);
}

void CCCountdown::setOpacity(unsigned __int8 opacity) {
    cocos2d::CCSprite::setOpacity(opacity);
    if (m_progressTimer && m_progressTimer->m_pSprite) {
        m_progressTimer->m_pSprite->setOpacity(opacity);
    }
}

void CCCountdown::startCountdown(cocos2d::CCNode* target, cocos2d::SEL_CallFunc callback, int seconds) {
    m_targetNode = target;
    m_callbackSelector = callback;
    m_seconds = seconds;

    CCString* str = CCString::createWithFormat("%i", seconds);
    m_countdownLabel->setString(str->getCString());
    m_progressTimer->setPercentage(100.0f);

    cocos2d::CCProgressFromTo* progress = cocos2d::CCProgressFromTo::create(1.0f, 100.0f, 0.0f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(CCCountdown::lapFinished));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(progress, callFunc, nullptr);
    sequence->setTag(0);
    m_progressTimer->runAction(sequence);
}

CCCountdown::~CCCountdown() {
    // Destructor
}