#pragma once

#include "Platform/cocos2d.h"
#include <map>

class EnterEffectObject;
class GJBaseGameLayer;

struct EnterEffectAnimValue {
    int key;
    float fromValue;
    float toValue;
    float duration;
    float elapsed;
    int easingType;
    float easingRate;
    int easingBuffer;
};

class EnterEffectInstance {
public:
    float getValue(int key);
    void loadTransitions(EnterEffectObject* object, float time);
    void loadValuesFromObject(EnterEffectObject* object);
    void setValue(int key, float value);
    void updateTransitions(float dt, GJBaseGameLayer* layer);
    ~EnterEffectInstance();

private:
    struct HSVValue {
        float h;
        float s;
        float v;
        bool absoluteSaturation;
        bool absoluteBrightness;
    };

    float m_length;
    float m_lengthVariance;
    float m_offset;
    float m_offsetVariance;
    float m_modFront;
    float m_modBack;
    float m_deadzone;
    float m_moveDistance;
    float m_moveDistanceVariance;
    float m_moveAngle;
    float m_moveAngleVariance;
    float m_moveX;
    float m_moveXVariance;
    float m_moveY;
    float m_moveYVariance;
    float m_scaleX;
    float m_scaleXVariance;
    float m_scaleY;
    float m_scaleYVariance;
    float m_rotation;
    float m_rotationVariance;
    float m_tint;
    float m_unk074;
    float m_toOpacity;
    float m_fromOpacity;
    float m_offsetY;
    float m_offsetYVariance;
    float m_relativeFade;
    float m_hue;
    float m_saturation;
    float m_value;
    
    HSVValue m_hsvValue;
    
    cocos2d::CCArray* m_easeIndices;
    std::map<int, EnterEffectAnimValue> m_enterEffectAnimMap;
};