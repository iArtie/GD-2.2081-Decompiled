#include "UI/SliderTouchLogic.h"

bool SliderTouchLogic::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (!cocos2d::CCMenu::ccTouchBegan(touch, event))
        return false;

    m_activateThumb = true;
    cocos2d::CCPoint touchPoint = cocos2d::CCNode::convertTouchToNodeSpace(touch);
    m_position = touchPoint - m_thumb->getPosition();

    if (m_slider)
        m_slider->sliderBegan();
    return true;
}

void SliderTouchLogic::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_activateThumb = false;
    cocos2d::CCMenu::ccTouchEnded(touch, event);
    if (m_slider)
        m_slider->sliderEnded();
}

void SliderTouchLogic::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    cocos2d::CCPoint touchPoint = cocos2d::CCNode::convertTouchToNodeSpace(touch);
    cocos2d::CCPoint relativePos = touchPoint - m_position;

    if (m_rotated) {
        float minVal = -m_length * 0.5f;
        float maxVal = m_length * 0.5f;
        float yVal;
        if (relativePos.y < minVal)
            yVal = minVal;
        else if (relativePos.y > maxVal)
            yVal = maxVal;
        else
            yVal = relativePos.y;
        m_thumb->setPosition(cocos2d::CCPoint(0.0f, yVal));
    } else {
        float minVal = -m_length * 0.5f;
        float maxVal = m_length * 0.5f;
        float xVal;
        if (relativePos.x < minVal)
            xVal = minVal;
        else if (relativePos.x > maxVal)
            xVal = maxVal;
        else
            xVal = relativePos.x;
        m_thumb->setPosition(cocos2d::CCPoint(xVal, 0.0f));
    }

    if (m_activateThumb)
        m_thumb->activate();

    if (m_slider)
        m_slider->updateBar();
}

void SliderTouchLogic::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addPrioTargetedDelegate(this, -500, true);
}

void SliderTouchLogic::setMaxOffset(float offset) {
    m_length = m_thumb->getScale() * offset;
    m_thumb->setMaxOffset(offset);
}

void SliderTouchLogic::setRotated(bool rotated) {
    m_rotated = rotated;
    m_thumb->setRotated(rotated);
}

SliderTouchLogic::~SliderTouchLogic() {
}