#include "Models/KeyframeAnimTriggerObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Objects/EffectGameObject.h"

KeyframeAnimTriggerObject* KeyframeAnimTriggerObject::create() {
    auto obj = new KeyframeAnimTriggerObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool KeyframeAnimTriggerObject::init() {
    return EffectGameObject::init("edit_eKeyframeBtn_001.png");
}

std::string KeyframeAnimTriggerObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = EffectGameObject::getSaveString(layer);
    
    char buffer[512];
    int pos = 0;
    
    auto appendFloat = [&](float value, int key) {
        if (value != 0.0f) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%i,%f", key, value);
        }
    };
    
    appendFloat(m_timeMod, 520);
    appendFloat(m_positionXMod, 521);
    appendFloat(m_positionYMod, 545);
    appendFloat(m_rotationMod, 522);
    appendFloat(m_scaleXMod, 523);
    appendFloat(m_scaleYMod, 546);
    
    result += buffer;
    
    return result;
}

KeyframeAnimTriggerObject::~KeyframeAnimTriggerObject() {
}