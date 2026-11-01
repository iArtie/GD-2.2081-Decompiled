#include "Cocos/CCCircleAlert.h"
#include "Platform/cocos2d.h"

CCCircleAlert* CCCircleAlert::create(float duration, float startRadius, float endRadius) {
    CCCircleAlert* alert = new CCCircleAlert();
    if (alert && alert->init(duration, startRadius, endRadius)) {
        alert->autorelease();
        return alert;
    }
    delete alert;
    return nullptr;
}

bool CCCircleAlert::init(float duration, float startRadius, float endRadius) {
    m_color.r = 255;
    m_color.g = 255;
    m_color.b = 255;
    m_radius = startRadius;
    m_opacity = 0;

    cocos2d::CCActionTween* radiusTween = cocos2d::CCActionTween::create(
        "radius",
        duration,
        startRadius,
        endRadius
    );

    cocos2d::CCActionTween* opacityUp = cocos2d::CCActionTween::create(
        "opacity",
        duration - 0.3f,
        0.0f,
        150.0f
    );

    cocos2d::CCActionTween* opacityDown = cocos2d::CCActionTween::create(
        "opacity",
        0.3f,
        150.0f,
        0.0f
    );

    cocos2d::CCSequence* opacitySequence = cocos2d::CCSequence::create(opacityUp, opacityDown, nullptr);
    cocos2d::CCSpawn* spawn = cocos2d::CCSpawn::create(radiusTween, opacitySequence, nullptr);

    this->runAction(spawn);

    return true;
}

CCCircleAlert::~CCCircleAlert() {
    // Destructor
}