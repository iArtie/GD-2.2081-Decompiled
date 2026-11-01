#include "EnterEffectObject.h"
#include "GJBaseGameLayer.h"
#include "GameToolbox.h"

EnterEffectObject* EnterEffectObject::create(const char* frame) {
    EnterEffectObject* ret = new EnterEffectObject();
    if (ret && ret->init(frame)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void EnterEffectObject::customSetup() {
    EffectGameObject::customSetup();
    if ((m_objectID - 3011) <= 4) {
        resetEnterAnimValues();
    }
}

std::string EnterEffectObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
    std::string result = saveString;
    
    if (m_enterType) {
        result += ",�," + std::to_string(m_enterType); // 0xD9
    }
    
    if (m_enterChannel) {
        result += ",�," + std::to_string(m_enterChannel); // 0x158
    }
    
    // Check object IDs 3006-3011 (0xBBE-0xBC0, 0xBC1-0xBC7, 0xBC9-0xBCC, 0xBCD, 0xBCF, 0xBD0)
    if ((m_objectID >= 3006 && m_objectID <= 3011) ||
        (m_objectID >= 3009 && m_objectID <= 3015) ||
        (m_objectID >= 3017 && m_objectID <= 3020) ||
        m_objectID == 3021 || m_objectID == 3023 || m_objectID == 3024) {
        
        if (m_length) {
            result += ",�," + std::to_string(m_length); // 0xDE
        }
        if (m_lengthVariance) {
            result += ",�," + std::to_string(m_lengthVariance); // 0xDF
        }
        if (m_offset) {
            result += ",�," + std::to_string(m_offset); // 0xDC
        }
        if (m_offsetVariance) {
            result += ",�," + std::to_string(m_offsetVariance); // 0xDD
        }
        if (m_offsetY) {
            result += ",�," + std::to_string(m_offsetY); // 0xFC
        }
        if (m_offsetYVariance) {
            result += ",�," + std::to_string(m_offsetYVariance); // 0xFD
        }
        if (m_moveDistance) {
            result += ",�," + std::to_string(m_moveDistance); // 0xDA
        }
        if (m_moveDistanceVariance) {
            result += ",�," + std::to_string(m_moveDistanceVariance); // 0xDB
        }
        if (m_moveX) {
            result += ",�," + std::to_string(m_moveX); // 0xED
        }
        if (m_moveXVariance) {
            result += ",�," + std::to_string(m_moveXVariance); // 0xEE
        }
        if (m_moveY) {
            result += ",�," + std::to_string(m_moveY); // 0xEF
        }
        if (m_moveYVariance) {
            result += ",�," + std::to_string(m_moveYVariance); // 0xF0
        }
        if (m_easingInType) {
            result += ",�," + std::to_string(m_easingInType); // 0xF2
        }
        if (m_easingInRate != 0.0f) {
            result += ",�," + std::to_string(m_easingInRate); // 0xF3
        }
        if (m_easingOutType) {
            result += ",�," + std::to_string(m_easingOutType); // 0xF8
        }
        if (m_easingOutRate != 0.0f) {
            result += ",�," + std::to_string(m_easingOutRate); // 0xF9
        }
        if (m_moveAngle) {
            result += ",�," + std::to_string(m_moveAngle); // 0xE7
        }
        if (m_moveAngleVariance) {
            result += ",�," + std::to_string(m_moveAngleVariance); // 0xE8
        }
        if (m_relative) {
            result += ",�," + std::to_string(m_relative); // 0x11F
        }
        if (m_relativeFade != 0.0f) {
            result += ",�," + std::to_string(m_relativeFade); // 0x120
        }
        if (m_areaScaleX != 0.0f) {
            result += ",�," + std::to_string(m_areaScaleX); // 0xE9
        }
        if (m_areaScaleXVariance != 0.0f) {
            result += ",�," + std::to_string(m_areaScaleXVariance); // 0xEA
        }
        if (m_areaScaleY != 0.0f) {
            result += ",�," + std::to_string(m_areaScaleY); // 0xEB
        }
        if (m_areaScaleYVariance != 0.0f) {
            result += ",�," + std::to_string(m_areaScaleYVariance); // 0xEC
        }
        if (m_tintChannelID) {
            result += ",�," + std::to_string(m_tintChannelID); // 0x104
        }
        if (m_property224) {
            result += ",�," + std::to_string(m_property224); // 0xE0
        }
        if (m_directionType) {
            result += ",�," + std::to_string(m_directionType); // 0x106
        }
        if (m_targetGroupID) {
            result += ",3," + std::to_string(m_targetGroupID);
        }
        if (m_centerGroupID) {
            result += ",G," + std::to_string(m_centerGroupID);
        }
        if (m_specialTarget) {
            result += ",�," + std::to_string(m_specialTarget); // 0x21A
        }
        if (m_xyMode) {
            result += ",�," + std::to_string(m_xyMode); // 0xF1
        }
        if (m_easeOutEnabled) {
            result += ",�," + std::to_string(m_easeOutEnabled); // 0x105
        }
        if (m_modFront != 0.0f) {
            result += ",�," + std::to_string(m_modFront); // 0x107
        }
        if (m_modBack != 0.0f) {
            result += ",�," + std::to_string(m_modBack); // 0x108
        }
        if (m_areaTint != 0.0f) {
            result += ",�," + std::to_string(m_areaTint); // 0x109
        }
        if (m_property285 != 0.0f) {
            result += ",�," + std::to_string(m_property285); // 0x11D
        }
        if (m_effectID) {
            result += ",�," + std::to_string(m_effectID); // 0xE1
        }
        if (m_areaRotation != 0.0f) {
            result += ",�," + std::to_string(m_areaRotation); // 0x10E
        }
        if (m_areaRotationVariance != 0.0f) {
            result += ",�," + std::to_string(m_areaRotationVariance); // 0x10F
        }
        if (m_toOpacity != 0.0f) {
            result += ",�," + std::to_string(m_toOpacity); // 0x113
        }
        if (m_fromOpacity != 0.0f) {
            result += ",�," + std::to_string(m_fromOpacity); // 0x11E
        }
        if (m_inbound) {
            result += ",�," + std::to_string(m_inbound); // 0x114
        }
        if (m_targetPlayer1) {
            result += ",�," + std::to_string(m_targetPlayer1); // 0x8A
        }
        if (m_targetPlayer2) {
            result += ",�," + std::to_string(m_targetPlayer2); // 0xC8
        }
        if (m_followCPP) {
            result += ",�," + std::to_string(m_followCPP); // 0xC9
        }
        if (m_pulseMainOnly) {
            result += ",A," + std::to_string(m_pulseMainOnly);
        }
        if (m_pulseDetailOnly) {
            result += ",B," + std::to_string(m_pulseDetailOnly);
        }
        if (m_deadzone != 0.0f) {
            result += ",�," + std::to_string(m_deadzone); // 0x11A
        }
        if (m_twoDirections) {
            result += ",�," + std::to_string(m_twoDirections); // 0x11B
        }
        if (m_priority) {
            result += ",�," + std::to_string(m_priority); // 0x155
        }
        if (m_hsvEnabled) {
            result += ",�," + std::to_string(m_hsvEnabled); // 0x116
        }
        if (m_duration != 0.0f) {
            result += ",�," + std::to_string(m_duration); // 0xA
        }
        if (m_useEffectID) {
            result += ",�," + std::to_string(m_useEffectID); // 0x163
        }
        if (m_dontEditAreaParent) {
            result += ",�," + std::to_string(m_dontEditAreaParent); // 0x21B
        }
        if (m_hsvEnabled) {
            // Check if HSV values are non-default
            if (m_hsvValue.h != 0.0f || m_hsvValue.s != 0.0f || m_hsvValue.v != 0.0f ||
                m_hsvValue.absoluteSaturation || m_hsvValue.absoluteBrightness) {
                result += ",1," + GameToolbox::stringFromHSV(m_hsvValue);
            }
        }
    }
    
    return result;
}

bool EnterEffectObject::init(const char* frame) {
    if (!EffectGameObject::init(frame))
        return false;
    
    m_objectType = 7; // EnterEffectObject
    m_unk390 = 45;
    return true;
}

void EnterEffectObject::resetEnterAnimValues() {
    m_length = -99;
    m_lengthVariance = -99;
    m_offset = -99;
    m_offsetVariance = -99;
    m_offsetY = -99;
    m_offsetYVariance = -99;
    m_moveDistance = -99;
    m_moveDistanceVariance = -99;
    m_moveAngle = -99;
    m_moveAngleVariance = -99;
    m_moveX = -99;
    m_moveXVariance = -99;
    m_moveY = -99;
    m_moveYVariance = -99;
    m_modFront = -99.0f;
    m_modBack = -99.0f;
    m_deadzone = -99.0f;
    m_areaScaleX = -99.0f;
    m_areaScaleXVariance = -99.0f;
    m_areaScaleY = -99.0f;
    m_areaScaleYVariance = -99.0f;
    m_areaRotation = -99.0f;
    m_areaRotationVariance = -99.0f;
    m_areaTint = -99.0f;
    m_property285 = -99.0f;
    m_toOpacity = -99.0f;
    m_fromOpacity = -99.0f;
    m_relativeFade = -99.0f;
}

EnterEffectObject::~EnterEffectObject() {
    // Base class destructor handles cleanup
}