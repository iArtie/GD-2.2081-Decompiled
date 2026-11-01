#include "CCAlertCircle.h"
#include "CCCircleAlert.h"
#include "cocos2d.h"

CCAlertCircle* CCAlertCircle::create() {
    CCAlertCircle* circle = new CCAlertCircle();
    if (circle && circle->init()) {
        circle->autorelease();
        return circle;
    }
    delete circle;
    return nullptr;
}

void CCAlertCircle::draw() {
    cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);
    cocos2d::ccDrawColor4B(
        m_circleAlert->m_color.r,
        m_circleAlert->m_color.g,
        m_circleAlert->m_color.b,
        m_circleAlert->m_opacity
    );
    cocos2d::ccDrawCircle(
        cocos2d::CCPoint(0.0f, 0.0f),
        m_circleAlert->m_radius,
        0.0f,
        30,
        false
    );
    cocos2d::ccGLBlendFunc(GL_ONE, GL_ZERO);
}

bool CCAlertCircle::init() {
    m_circleAlert = CCCircleAlert::create(100.0f, 0.1f, 0.9f);
    m_circleAlert->retain();
    
    cocos2d::CCAction* sequence = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.9f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(CCAlertCircle::removeFromParentAndCleanup)),
        nullptr
    );
    this->runAction(sequence);
    
    return true;
}

CCAlertCircle::~CCAlertCircle() {
    CC_SAFE_RELEASE_NULL(m_circleAlert);
}