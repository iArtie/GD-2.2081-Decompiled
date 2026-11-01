#include "UI/Slider.h"

bool Slider::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    return m_touchLogic->ccTouchBegan(touch, event);
}

void Slider::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_touchLogic->ccTouchEnded(touch, event);
}

void Slider::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_touchLogic->ccTouchMoved(touch, event);
}

Slider* Slider::create(cocos2d::CCNode* target, cocos2d::SEL_MenuHandler handler) {
    return (Slider*)Slider::create(target, handler.ptr, handler.adj, 1.0f);
}

void Slider::disableSlider() {
    if (m_enabled) {
        m_enabled = false;
        m_touchLogic->setTouchEnabled(false);
        setValue(0.0f);
        cocos2d::ccColor3B color = {150, 150, 150};
        m_sliderBar->setColor(color);
        m_groove->setColor(color);
        cocos2d::CCNode* thumbImage = m_touchLogic->m_thumb->getNormalImage();
        if (thumbImage)
            ((cocos2d::CCSprite*)thumbImage)->setColor(color);
    }
}

void Slider::disableTouch() {
    m_touchLogic->setTouchEnabled(false);
}

void Slider::enableSlider() {
    if (!m_enabled) {
        m_enabled = true;
        m_touchLogic->setTouchEnabled(true);
        m_sliderBar->setColor(cocos2d::ccc3(255, 255, 255));
        m_groove->setColor(cocos2d::ccc3(255, 255, 255));
        cocos2d::CCNode* thumbImage = m_touchLogic->m_thumb->getNormalImage();
        if (thumbImage)
            ((cocos2d::CCSprite*)thumbImage)->setColor(cocos2d::ccc3(255, 255, 255));
    }
}

bool Slider::getLiveDragging() {
    return m_touchLogic->m_activateThumb;
}

SliderThumb* Slider::getThumb() {
    return m_touchLogic->m_thumb;
}

float Slider::getValue() {
    return getThumb()->getValue();
}

void Slider::hideGroove(bool visibility) {
    m_groove->setVisible(!visibility);
}

void Slider::setBarVisibility(bool visibility) {
    m_sliderBar->setVisible(visibility);
}

void Slider::setLiveDragging(bool activateThumb) {
    m_touchLogic->m_activateThumb = activateThumb;
}

void Slider::setMaxOffset(float offset) {
    m_touchLogic->setMaxOffset(offset);
}

void Slider::setRotated(bool rotated) {
    m_touchLogic->setRotated(rotated);
    float rotation = rotated ? 90.0f : 0.0f;
    m_groove->setRotation(rotation);
}

void Slider::setValue(float val) {
    getThumb()->setValue(val);
    updateBar();
}

void Slider::sliderBegan() {
    if (m_delegate)
        m_delegate->sliderBegan(this);
}

void Slider::sliderEnded() {
    if (m_delegate)
        m_delegate->sliderEnded(this);
}

void Slider::updateBar() {
    float value = getThumb()->getValue();
    float barWidth = value * m_width;
    if (m_width <= barWidth)
        barWidth = m_width;
    cocos2d::CCRect rect(0.0f, 0.0f, barWidth, m_height);
    m_sliderBar->setTextureRect(rect);
}

Slider::~Slider() {
}