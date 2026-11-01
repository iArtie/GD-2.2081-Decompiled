#include "CCLightStrip.h"
#include "cocos2d.h"

CCLightStrip* CCLightStrip::create(float width, float toWidth, float toHeight, float duration, float delay) {
    CCLightStrip* strip = new CCLightStrip();
    if (strip && strip->init(width, toWidth, toHeight, duration, delay)) {
        strip->autorelease();
        return strip;
    }
    delete strip;
    return nullptr;
}

void CCLightStrip::draw() {
    if (!this->isVisible()) {
        return;
    }

    cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE);
    cocos2d::ccDrawColor4B(m_color.r, m_color.g, m_color.b, static_cast<unsigned char>(m_opacity));

    float baseWidth = m_objectWidth + ((m_width - m_objectWidth) * 0.25f);

    cocos2d::CCPoint points[4];
    points[0] = cocos2d::CCPoint(-baseWidth * 0.5f, 0.0f);
    points[1] = cocos2d::CCPoint(baseWidth * 0.5f, 0.0f);
    points[2] = cocos2d::CCPoint(m_width * 0.5f, m_height);
    points[3] = cocos2d::CCPoint(-m_width * 0.5f, m_height);

    cocos2d::ccColor4F color = {
        static_cast<float>(m_color.r) / 255.0f,
        static_cast<float>(m_color.g) / 255.0f,
        static_cast<float>(m_color.b) / 255.0f,
        m_opacity / 255.0f
    };

    cocos2d::ccDrawSolidPoly(points, 4, color);
    cocos2d::ccGLBlendFunc(GL_ONE, GL_ZERO);
}

bool CCLightStrip::init(float width, float toWidth, float toHeight, float duration, float delay) {
    m_toWidth = toWidth;
    m_toHeight = toHeight;
    m_objectWidth = width;
    m_duration = duration;
    m_width = width;
    m_opacity = 255.0f;
    m_height = 1.0f;
    m_color.r = 255;
    m_color.g = 255;
    m_color.b = 255;

    this->setVisible(false);

    cocos2d::CCDelayTime* delayAction = cocos2d::CCDelayTime::create(delay);
    cocos2d::CCShow* show = cocos2d::CCShow::create();

    cocos2d::CCActionTween* heightTween = cocos2d::CCActionTween::create(
        "height",
        duration,
        m_height,
        toHeight
    );
    cocos2d::CCEaseOut* heightEase = cocos2d::CCEaseOut::create(heightTween, 2.0f);

    cocos2d::CCActionTween* widthTween = cocos2d::CCActionTween::create(
        "width",
        duration,
        m_width,
        m_toWidth
    );
    cocos2d::CCEaseOut* widthEase = cocos2d::CCEaseOut::create(widthTween, 2.0f);

    cocos2d::CCSpawn* spawn = cocos2d::CCSpawn::create(heightEase, widthEase, nullptr);
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(delayAction, show, spawn, nullptr);

    this->runAction(sequence);

    return true;
}

void CCLightStrip::updateTweenAction(float value, const char* keyword) {
    std::string key(keyword);

    if (key == "height") {
        m_height = value;
    } else if (key == "width") {
        m_width = value;
    } else if (key == "opacity") {
        m_opacity = value;
    }
}

CCLightStrip::~CCLightStrip() {
    // Destructor
}