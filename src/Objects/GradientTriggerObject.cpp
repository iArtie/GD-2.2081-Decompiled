#include "Objects/GradientTriggerObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Objects/EffectGameObject.h"

GradientTriggerObject* GradientTriggerObject::create() {
    auto obj = new GradientTriggerObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool GradientTriggerObject::init() {
    if (!EffectGameObject::init("edit_eGradientBtn_001.png")) {
        return false;
    }
    
    m_blendingLayer = 1;
    m_blendingMode = 0;
    m_upBottomLeftID = 0;
    m_downBottomRightID = 0;
    m_leftTopLeftID = 0;
    m_rightTopRightID = 0;
    m_vertexMode = 0;
    m_disable = 0;
    m_disableAll = 0;
    m_gradientID = 0;
    m_previewOpacity = 0.0f;
    
    return true;
}

std::string GradientTriggerObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = EffectGameObject::getSaveString(layer);
    
    char buffer[512];
    int pos = 0;
    
    auto appendInt = [&](int value) {
        if (value) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%i", value);
        }
    };
    
    auto appendFloat = [&](float value) {
        if (value != 0.0f) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%f", value);
        }
    };
    
    if (m_blendingMode) {
        appendInt(174);
        appendInt(m_blendingMode);
    }
    
    if (m_blendingLayer) {
        appendInt(202);
        appendInt(m_blendingLayer);
    }
    
    if (m_upBottomLeftID) {
        appendInt(203);
        appendInt(m_upBottomLeftID);
    }
    
    if (m_downBottomRightID) {
        appendInt(204);
        appendInt(m_downBottomRightID);
    }
    
    if (m_leftTopLeftID) {
        appendInt(205);
        appendInt(m_leftTopLeftID);
    }
    
    if (m_rightTopRightID) {
        appendInt(206);
        appendInt(m_rightTopRightID);
    }
    
    if (m_vertexMode) {
        appendInt(207);
        appendInt(m_vertexMode);
    }
    
    if (m_disable) {
        appendInt(208);
        appendInt(m_disable);
    }
    
    if (m_disableAll) {
        appendInt(508);
        appendInt(m_disableAll);
    }
    
    if (m_gradientID) {
        appendInt(209);
        appendInt(m_gradientID);
    }
    
    if (m_previewOpacity != 0.0f) {
        appendInt(456);
        appendFloat(m_previewOpacity);
    }
    
    result += buffer;
    
    return result;
}

GradientTriggerObject::~GradientTriggerObject() {
}