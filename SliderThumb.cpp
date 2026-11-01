#include "SliderThumb.h"

float SliderThumb::getValue() {
    float pos = m_vertical ? getPosition().y : getPosition().x;
    float scale = getScale();
    return (pos + (m_length * scale * 0.5f)) / (m_length * scale);
}

void SliderThumb::setMaxOffset(float offset) {
    float value = getValue();
    m_length = offset;
    setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    setValue(value);
}

void SliderThumb::setRotated(bool rotated) {
    float value = getValue();
    m_vertical = rotated;
    setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    setValue(value);
}

void SliderThumb::setValue(float value) {
    cocos2d::CCPoint newPos;
    if (m_vertical) {
        float x = getPosition().x;
        float lengthScale = m_length * getScale();
        newPos = cocos2d::CCPoint(x, -(lengthScale * 0.5f - lengthScale * value));
    } else {
        float lengthScale = m_length * getScale();
        float y = getPosition().y;
        newPos = cocos2d::CCPoint(-(lengthScale * 0.5f - lengthScale * value), y);
    }
    setPosition(newPos);
}

SliderThumb::~SliderThumb() {
}