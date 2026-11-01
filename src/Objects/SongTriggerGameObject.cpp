#include "Objects/SongTriggerGameObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Objects/GameObject.h"

SongTriggerGameObject* SongTriggerGameObject::create(const char* frame) {
    SongTriggerGameObject* returnVal = new SongTriggerGameObject();
    if (returnVal->init(frame)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

std::string SongTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = SFXTriggerGameObject::getSaveString(layer);
    if (m_prep) {
        saveString += ",";
        saveString += std::to_string(399);
        saveString += ",";
        saveString += std::to_string(m_prep);
    }
    if (m_loadPrep) {
        saveString += ",";
        saveString += std::to_string(400);
        saveString += ",";
        saveString += std::to_string(m_loadPrep);
    }
    if (m_songChannel) {
        saveString += ",";
        saveString += std::to_string(432);
        saveString += ",";
        saveString += std::to_string(m_songChannel);
    }
    return saveString;
}

SongTriggerGameObject::~SongTriggerGameObject() {
}