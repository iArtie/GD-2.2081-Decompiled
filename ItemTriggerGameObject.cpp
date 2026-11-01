#include "ItemTriggerGameObject.h"
#include "GJBaseGameLayer.h"
#include "EffectGameObject.h"

ItemTriggerGameObject* ItemTriggerGameObject::create(const char* frame) {
    auto obj = new ItemTriggerGameObject();
    if (obj && obj->init(frame)) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool ItemTriggerGameObject::init(const char* frame) {
    return EffectGameObject::init(frame);
}

void ItemTriggerGameObject::customSetup() {
    EffectGameObject::customSetup();
    
    if (m_objectID == 3620) {
        m_resultType3 = 0;
        m_resultType1 = 3;
        m_resultType2 = 3;
    }
}

std::string ItemTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = EffectGameObject::getSaveString(layer);
    
    char buffer[512];
    int pos = 0;
    
    auto appendInt = [&](int value, int key) {
        if (value) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%i,%i", key, value);
        }
    };
    
    auto appendFloat = [&](float value, int key) {
        if (value != 0.0f) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%i,%f", key, value);
        }
    };
    
    switch (m_objectID) {
        case 3620:
            appendInt(m_itemID, 80);
            appendInt(m_itemID2, 95);
            appendInt(m_targetGroupID, 51);
            appendInt(m_centerGroupID, 71);
            appendInt(m_item1Mode, 476);
            appendInt(m_item2Mode, 477);
            appendFloat(m_mod1, 479);
            appendFloat(m_mod2, 483);
            appendInt(m_resultType1, 480);
            appendInt(m_resultType2, 481);
            appendInt(m_resultType3, 482);
            appendFloat(m_tolerance, 484);
            appendInt(m_roundType1, 485);
            appendInt(m_roundType2, 486);
            appendInt(m_signType1, 578);
            appendInt(m_signType2, 579);
            break;
            
        case 3641:
            appendInt(m_itemID, 80);
            appendInt(m_persistent, 491);
            appendInt(m_targetAll, 492);
            appendInt(m_reset, 493);
            appendInt(m_timer, 494);
            break;
            
        case 3619:
            appendInt(m_itemID, 80);
            appendInt(m_itemID2, 95);
            appendInt(m_item1Mode, 476);
            appendInt(m_item2Mode, 477);
            appendInt(m_targetItemMode, 478);
            appendInt(m_targetGroupID, 51);
            appendFloat(m_mod1, 479);
            appendInt(m_resultType1, 480);
            appendInt(m_resultType2, 481);
            appendInt(m_resultType3, 482);
            appendInt(m_roundType1, 485);
            appendInt(m_roundType2, 486);
            appendInt(m_signType1, 578);
            appendInt(m_signType2, 579);
            break;
    }
    
    result += buffer;
    
    return result;
}

ItemTriggerGameObject::~ItemTriggerGameObject() {
}