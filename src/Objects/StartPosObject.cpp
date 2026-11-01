#include "Objects/StartPosObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Models/LevelSettingsObject.h"
#include "Utils/GameToolbox.h"

StartPosObject* StartPosObject::create() {
    StartPosObject* returnVal = new StartPosObject();
    if (returnVal->init()) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

std::string StartPosObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
    if (m_startSettings) {
        saveString += ",";
        saveString += m_startSettings->getSaveString();
    }
    return saveString;
}

bool StartPosObject::init() {
    if (!EffectGameObject::init("edit_eStartPosBtn_001.png"))
        return false;

    LevelSettingsObject* settings = LevelSettingsObject::create();
    setSettings(settings);
    m_startSettings->m_startsWithStartPos = true;
    return true;
}

void StartPosObject::loadSettingsFromString(std::string objectString) {
    cocos2d::CCDictionary* dict = GameToolbox::stringSetupToDict(objectString, ",");
    LevelSettingsObject* settings = LevelSettingsObject::objectFromDict(dict);
    setSettings(settings);
}

void StartPosObject::setSettings(LevelSettingsObject* settings) {
    if (m_startSettings != settings) {
        if (settings)
            settings->retain();
        if (m_startSettings)
            m_startSettings->release();
        m_startSettings = settings;
    }
}

StartPosObject::~StartPosObject() {
}