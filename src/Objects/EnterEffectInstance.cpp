#include "Objects/EnterEffectInstance.h"
#include "Objects/EnterEffectObject.h"
#include "Layers/GJBaseGameLayer.h"

float EnterEffectInstance::getValue(int key) {
    switch (key) {
        case 0:  return m_length;
        case 1:  return m_lengthVariance;
        case 2:  return m_offset;
        case 3:  return m_offsetVariance;
        case 4:  return m_modFront;
        case 5:  return m_modBack;
        case 6:  return m_deadzone;
        case 7:  return m_moveDistance;
        case 8:  return m_moveDistanceVariance;
        case 9:  return m_moveAngle;
        case 10: return m_moveAngleVariance;
        case 11: return m_moveX;
        case 12: return m_moveXVariance;
        case 13: return m_moveY;
        case 14: return m_moveYVariance;
        case 15: return m_scaleX;
        case 16: return m_scaleXVariance;
        case 17: return m_scaleY;
        case 18: return m_scaleYVariance;
        case 19: return m_rotation;
        case 20: return m_rotationVariance;
        case 21: return m_tint;
        case 22: return m_unk074;
        case 23: return m_toOpacity;
        case 24: return m_fromOpacity;
        case 25: return m_offsetY;
        case 26: return m_offsetYVariance;
        case 27: return m_relativeFade;
        default: return 0.0f;
    }
}

void EnterEffectInstance::loadTransitions(EnterEffectObject* object, float time) {
    m_length = object->m_length;
    if (m_length != -99) {
        animateValue(0, m_length, (float)m_length, object->m_duration, 
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_lengthVariance = object->m_lengthVariance;
    if (m_lengthVariance != -99) {
        animateValue(1, m_lengthVariance, (float)m_lengthVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_offset = object->m_offset;
    if (m_offset != -99) {
        animateValue(2, m_offset, (float)m_offset, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_offsetVariance = object->m_offsetVariance;
    if (m_offsetVariance != -99) {
        animateValue(3, m_offsetVariance, (float)m_offsetVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_modFront = object->m_modFront;
    if (m_modFront != -99.0f) {
        animateValue(4, m_modFront, m_modFront, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_modBack = object->m_modBack;
    if (m_modBack != -99.0f) {
        animateValue(5, m_modBack, m_modBack, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_deadzone = object->m_deadzone;
    if (m_deadzone != -99.0f) {
        animateValue(6, m_deadzone, m_deadzone, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveDistance = object->m_moveDistance;
    if (m_moveDistance != -99) {
        animateValue(7, m_moveDistance, (float)m_moveDistance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveDistanceVariance = object->m_moveDistanceVariance;
    if (m_moveDistanceVariance != -99) {
        animateValue(8, m_moveDistanceVariance, (float)m_moveDistanceVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveAngle = object->m_moveAngle;
    if (m_moveAngle != -99) {
        animateValue(9, m_moveAngle, (float)m_moveAngle, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveAngleVariance = object->m_moveAngleVariance;
    if (m_moveAngleVariance != -99) {
        animateValue(10, m_moveAngleVariance, (float)m_moveAngleVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveX = object->m_moveX;
    if (m_moveX != -99) {
        animateValue(11, m_moveX, (float)m_moveX, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveXVariance = object->m_moveXVariance;
    if (m_moveXVariance != -99) {
        animateValue(12, m_moveXVariance, (float)m_moveXVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveY = object->m_moveY;
    if (m_moveY != -99) {
        animateValue(13, m_moveY, (float)m_moveY, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_moveYVariance = object->m_moveYVariance;
    if (m_moveYVariance != -99) {
        animateValue(14, m_moveYVariance, (float)m_moveYVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_scaleX = object->m_areaScaleX;
    if (m_scaleX != -99.0f) {
        animateValue(15, m_scaleX, m_scaleX, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_scaleXVariance = object->m_areaScaleXVariance;
    if (m_scaleXVariance != -99.0f) {
        animateValue(16, m_scaleXVariance, m_scaleXVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_scaleY = object->m_areaScaleY;
    if (m_scaleY != -99.0f) {
        animateValue(17, m_scaleY, m_scaleY, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_scaleYVariance = object->m_areaScaleYVariance;
    if (m_scaleYVariance != -99.0f) {
        animateValue(18, m_scaleYVariance, m_scaleYVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_rotation = object->m_areaRotation;
    if (m_rotation != -99.0f) {
        animateValue(19, m_rotation, m_rotation, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_rotationVariance = object->m_areaRotationVariance;
    if (m_rotationVariance != -99.0f) {
        animateValue(20, m_rotationVariance, m_rotationVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_tint = object->m_areaTint;
    if (m_tint != -99.0f) {
        animateValue(21, m_tint, m_tint, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_unk074 = object->m_property285;
    if (m_unk074 != -99.0f) {
        animateValue(22, m_unk074, m_unk074, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_toOpacity = object->m_toOpacity;
    if (m_toOpacity != -99.0f) {
        animateValue(23, m_toOpacity, m_toOpacity, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_fromOpacity = object->m_fromOpacity;
    if (m_fromOpacity != -99.0f) {
        animateValue(24, m_fromOpacity, m_fromOpacity, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_offsetY = object->m_offsetY;
    if (m_offsetY != -99) {
        animateValue(25, m_offsetY, (float)m_offsetY, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_offsetYVariance = object->m_offsetYVariance;
    if (m_offsetYVariance != -99) {
        animateValue(26, m_offsetYVariance, (float)m_offsetYVariance, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    m_relativeFade = object->m_relativeFade;
    if (m_relativeFade != -99.0f) {
        animateValue(27, m_relativeFade, m_relativeFade, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
    
    if (object->m_hsvEnabled) {
        animateValue(28, m_hue, object->m_hsvValue.h, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
        animateValue(29, m_saturation, object->m_hsvValue.s, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
        animateValue(30, m_value, object->m_hsvValue.v, object->m_duration,
                    object->m_easingInType, object->m_easingInRate, object->m_easingInBuffer);
    }
}

void EnterEffectInstance::loadValuesFromObject(EnterEffectObject* object) {
    m_moveDistance = object->m_moveDistance;
    m_lengthVariance = object->m_lengthVariance;
    m_moveDistanceVariance = object->m_moveDistanceVariance;
    m_offset = object->m_offset;
    m_moveY = object->m_moveY;
    m_offsetVariance = object->m_offsetVariance;
    m_moveAngle = object->m_moveAngle;
    m_offsetY = object->m_offsetY;
    m_moveAngleVariance = object->m_moveAngleVariance;
    m_offsetYVariance = object->m_offsetYVariance;
    m_moveX = object->m_moveX;
    m_moveXVariance = object->m_moveXVariance;
    m_moveYVariance = object->m_moveYVariance;
    m_length = object->m_length;
    m_modFront = object->m_modFront;
    m_modBack = object->m_modBack;
    m_deadzone = object->m_deadzone;
    m_scaleX = object->m_areaScaleX;
    m_scaleXVariance = object->m_areaScaleXVariance;
    m_scaleY = object->m_areaScaleY;
    m_scaleYVariance = object->m_areaScaleYVariance;
    m_rotation = object->m_areaRotation;
    m_rotationVariance = object->m_areaRotationVariance;
    m_tint = object->m_areaTint;
    m_unk074 = object->m_property285;
    m_toOpacity = object->m_toOpacity;
    m_fromOpacity = object->m_fromOpacity;
    m_relativeFade = object->m_relativeFade;
    
    m_hsvValue.h = object->m_hsvValue.h;
    m_hsvValue.s = object->m_hsvValue.s;
    m_hsvValue.v = object->m_hsvValue.v;
    m_hsvValue.absoluteSaturation = object->m_hsvValue.absoluteSaturation;
    m_hsvValue.absoluteBrightness = object->m_hsvValue.absoluteBrightness;
    
    m_hue = m_hsvValue.h;
    m_saturation = m_hsvValue.s;
    m_value = m_hsvValue.v;
    
    // Clear animation map
    m_enterEffectAnimMap.clear();
}

void EnterEffectInstance::setValue(int key, float value) {
    switch (key) {
        case 0:  m_length = value; break;
        case 1:  m_lengthVariance = value; break;
        case 2:  m_offset = value; break;
        case 3:  m_offsetVariance = value; break;
        case 4:  m_modFront = value; break;
        case 5:  m_modBack = value; break;
        case 6:  m_deadzone = value; break;
        case 7:  m_moveDistance = value; break;
        case 8:  m_moveDistanceVariance = value; break;
        case 9:  m_moveAngle = value; break;
        case 10: m_moveAngleVariance = value; break;
        case 11: m_moveX = value; break;
        case 12: m_moveXVariance = value; break;
        case 13: m_moveY = value; break;
        case 14: m_moveYVariance = value; break;
        case 15: m_scaleX = value; break;
        case 16: m_scaleXVariance = value; break;
        case 17: m_scaleY = value; break;
        case 18: m_scaleYVariance = value; break;
        case 19: m_rotation = value; break;
        case 20: m_rotationVariance = value; break;
        case 21: m_tint = value; break;
        case 22: m_unk074 = value; break;
        case 23: m_toOpacity = value; break;
        case 24: m_fromOpacity = value; break;
        case 25: m_offsetY = value; break;
        case 26: m_offsetYVariance = value; break;
        case 27: m_relativeFade = value; break;
        case 28: m_hue = value; break;
        case 29: m_saturation = value; break;
        case 30: m_value = value; break;
        default: break;
    }
}

void EnterEffectInstance::updateTransitions(float dt, GJBaseGameLayer* layer) {
    std::map<int, EnterEffectAnimValue>::iterator it = m_enterEffectAnimMap.begin();
    
    while (it != m_enterEffectAnimMap.end()) {
        EnterEffectAnimValue& anim = it->second;
        anim.elapsed += dt;
        
        if (anim.elapsed >= anim.duration) {
            setValue(anim.key, anim.toValue);
            it = m_enterEffectAnimMap.erase(it);
            continue;
        }
        
        float progress = anim.elapsed / anim.duration;
        float easedValue = layer->getEnterEasingValue(progress, anim.easingType, anim.easingRate, anim.easingBuffer);
        float currentValue = anim.fromValue + (easedValue * (anim.toValue - anim.fromValue));
        setValue(anim.key, currentValue);
        
        ++it;
    }
    
    m_hsvValue.h = m_hue;
    m_hsvValue.s = m_saturation;
    m_hsvValue.v = m_value;
}

EnterEffectInstance::~EnterEffectInstance() {
    if (m_easeIndices) {
        m_easeIndices->release();
        m_easeIndices = nullptr;
    }
    m_enterEffectAnimMap.clear();
}