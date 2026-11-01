#include "Popups/Setup/SetupAreaAnimTriggerPopup.h"
#include "Objects/EffectGameObject.h"

SetupAreaAnimTriggerPopup* SetupAreaAnimTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    SetupAreaAnimTriggerPopup* popup = new SetupAreaAnimTriggerPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAreaAnimTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 320.0f, 260.0f, 2)) {
        return false;
    }

    return true;
}

void SetupAreaAnimTriggerPopup::onDeactivateAnimValue(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    SetupTriggerPopup::updateValue(tag, -99.0f);
    SetupTriggerPopup::updateInputNodeLabel(tag, "NA");
}

void SetupAreaAnimTriggerPopup::updateDefaultTriggerValues() {
    SetupTriggerPopup::updateDefaultTriggerValues();

    cocos2d::CCArray* keys = cocos2d::CCDictionary::allKeys(m_inputNodes);
    for (unsigned int i = 0; i < keys->count(); i++) {
        int tag = atoi(((cocos2d::CCString*)keys->objectAtIndex(i))->getCString());
        if (SetupTriggerPopup::getValue(tag) == -99.0f) {
            SetupTriggerPopup::updateInputNodeLabel(tag, "NA");
        }
    }

    updateTargetIDLabel();
}

void SetupAreaAnimTriggerPopup::updateTargetIDLabel() {
    cocos2d::CCObject* label = cocos2d::CCDictionary::objectForKey(
        m_inputLabels, cocos2d::CCString::createWithFormat("%i", 71)->getCString());

    if (label) {
        const char* text = (SetupTriggerPopup::getValue(355) == 0.0f) ? "GroupID" : "EffectID";
        ((cocos2d::CCLabelBMFont*)label)->setString(text);
    }
}

void SetupAreaAnimTriggerPopup::valueDidChange(int tag, float value) {
    if (tag == 241) {
        SetupTriggerPopup::toggleGroup(1, value == 0.0f);
        SetupTriggerPopup::toggleGroup(2, value != 0.0f);
    } else if (tag == 355) {
        updateTargetIDLabel();
    }
}

SetupAreaAnimTriggerPopup::~SetupAreaAnimTriggerPopup() {
}