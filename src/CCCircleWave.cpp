#include "CCCircleWave.h"
#include "CCCircleWaveDelegate.h"
#include "cocos2d.h"

void CCCircleWave::baseSetup(float radius) {
    m_color.r = 255;
    m_color.g = 255;
    m_color.b = 255;
    m_lineWidth = 2.0f;
    m_blendAdditive = true;
    m_circleMode = CircleMode::Filled;
    m_radius = radius;
    m_target = nullptr;
    m_opacity = 255.0f;
    m_opacityMod = 1.0f;
}

void CCCircleWave::draw() {
    if (m_blendAdditive) {
        cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE);
        if (m_circleMode == CircleMode::Filled) {
            goto draw_filled;
        }
    } else if (m_circleMode == CircleMode::Filled) {
        goto draw_filled;
    }

    glLineWidth(m_lineWidth);

draw_filled:
    float opacity = m_opacity * m_opacityMod;
    unsigned char intOpacity = 0;
    if (opacity > 0.0f) {
        if (opacity < 255.0f) {
            intOpacity = static_cast<unsigned char>(opacity);
        } else {
            intOpacity = 255;
        }
    }

    cocos2d::ccDrawColor4B(m_color.r, m_color.g, m_color.b, intOpacity);

    int segments = 10;
    if (m_radius >= 10.0f) {
        segments = 16;
        if (m_radius >= 20.0f) {
            segments = 20;
            if (m_radius >= 40.0f) {
                if (m_radius >= 200.0f) {
                    segments = 50;
                } else {
                    segments = 30;
                }
            }
        }
    }

    if (m_circleMode == CircleMode::Filled) {
        cocos2d::ccDrawFilledCircle(m_position, m_radius, 0.0f, segments);
        if (!m_blendAdditive) {
            return;
        }
        goto restore_blend;
    } else {
        cocos2d::ccDrawCircle(m_position, m_radius, 0.0f, segments, false);
        if (m_blendAdditive) {
            goto restore_blend;
        }
    }

    return;

restore_blend:
    cocos2d::ccGLBlendFunc(GL_ONE, GL_ZERO);
}

void CCCircleWave::followObject(cocos2d::CCNode* newTarget, bool staticPosition) {
    if (m_target) {
        m_target->release();
    }

    m_target = newTarget;
    if (newTarget) {
        newTarget->retain();
    }

    this->unschedule(schedule_selector(CCCircleWave::updatePosition));

    if (!staticPosition) {
        this->schedule(schedule_selector(CCCircleWave::updatePosition), 0.0f);
    }

    if (m_target) {
        this->setPosition(m_target->getPosition());
    }
}

void CCCircleWave::removeMeAndCleanup() {
    if (m_delegate) {
        m_delegate->circleWaveWillBeRemoved(this);
    }
    cocos2d::CCNode::removeMeAndCleanup();
}

void CCCircleWave::setPosition(const cocos2d::CCPoint* position) {
    cocos2d::CCNode::setPosition(cocos2d::CCPoint(0, 0));
    m_position = *position;
}

void CCCircleWave::updatePosition(float dt) {
    if (m_target) {
        this->setPosition(m_target->getPosition());
    }
}

void CCCircleWave::updateTweenAction(float value, const char* key) {
    std::string keyStr(key);

    if (keyStr == "opacity") {
        m_opacity = value;
    } else if (keyStr == "radius") {
        m_radius = value;
    }

    if (m_target) {
        this->setPosition(m_target->getPosition());
    }
}

CCCircleWave::~CCCircleWave() {
    CC_SAFE_RELEASE_NULL(m_target);
}