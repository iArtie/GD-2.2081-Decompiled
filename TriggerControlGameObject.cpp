#include "TriggerControlGameObject.h"
#include "GJBaseGameLayer.h"

TriggerControlGameObject* TriggerControlGameObject::create(const char* frame) {
    TriggerControlGameObject* returnVal = new TriggerControlGameObject();
    if (returnVal->init(frame)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool TriggerControlGameObject::init(const char* frame) {
    m_triggerControlFrame = frame;
    return EffectGameObject::init(frame);
}

std::string TriggerControlGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);

    if (m_targetGroupID) {
        saveString += ",";
        saveString += std::to_string(51);
        saveString += ",";
        saveString += std::to_string(m_targetGroupID);
    }
    if (m_targetControlID) {
        saveString += ",";
        saveString += std::to_string(535);
        saveString += ",";
        saveString += std::to_string(m_targetControlID);
    }
    if (m_customTriggerValue != GJActionCommand::Stop) {
        saveString += ",";
        saveString += std::to_string(580);
        saveString += ",";
        saveString += std::to_string((int)m_customTriggerValue);
    }
    return saveString;
}

void TriggerControlGameObject::updateTriggerControlFrame() {
    std::string newFrame = "edit_eStopMoverBtn_001.png";
    if (m_customTriggerValue == GJActionCommand::Pause) {
        newFrame = "edit_ePauseBtn_001.png";
    } else if (m_customTriggerValue == GJActionCommand::Resume) {
        newFrame = "edit_eResumeBtn_001.png";
    }

    if (m_triggerControlFrame != newFrame) {
        m_triggerControlFrame = newFrame;
        setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(newFrame.c_str()));
    }
}

TriggerControlGameObject::~TriggerControlGameObject() {
}