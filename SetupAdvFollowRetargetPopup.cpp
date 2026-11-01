#include "SetupAdvFollowRetargetPopup.h"
#include "AdvancedFollowTriggerObject.h"
#include "CCTextInputNode.h"

SetupAdvFollowRetargetPopup* SetupAdvFollowRetargetPopup::create(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects) {
    SetupAdvFollowRetargetPopup* popup = new SetupAdvFollowRetargetPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAdvFollowRetargetPopup::init(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 260.0f, 3)) {
        return false;
    }

    return true;
}

void SetupAdvFollowRetargetPopup::updateDefaultTriggerValues() {
    SetupTriggerPopup::updateDefaultTriggerValues();

    CCTextInputNode* inputNode = (CCTextInputNode*)cocos2d::CCDictionary::objectForKey(
        m_inputNodes, cocos2d::CCString::createWithFormat("%i", 71)->getCString());

    if (inputNode) {
        if (SetupTriggerPopup::getValue(138) != 0.0f) {
            inputNode->setString("P1");
        } else if (SetupTriggerPopup::getValue(200) != 0.0f) {
            inputNode->setString("P2");
        } else if (SetupTriggerPopup::getValue(201) != 0.0f) {
            inputNode->setString("C");
        }
    }
}

void SetupAdvFollowRetargetPopup::valueDidChange(int tag, float value) {
    if (tag != 138 && (tag < 138 || (unsigned int)(tag - 200) > 1)) {
        return;
    }

    SetupTriggerPopup::updateValue(71, 0.0f);
    std::string label = "0";

    if (value != 0.0f) {
        if (tag == 138) {
            label = "P1";
        } else if (tag == 200) {
            SetupTriggerPopup::updateValue(138, 0.0f);
            label = "P2";
            SetupTriggerPopup::updateValue(201, 0.0f);
        } else if (tag == 201) {
            SetupTriggerPopup::updateValue(138, 0.0f);
            SetupTriggerPopup::updateValue(200, 0.0f);
            label = "C";
        }
    }

    CCTextInputNode* inputNode = (CCTextInputNode*)cocos2d::CCDictionary::objectForKey(
        m_inputNodes, cocos2d::CCString::createWithFormat("%i", 71)->getCString());

    if (inputNode) {
        inputNode->setString(label.c_str());
    }
}

SetupAdvFollowRetargetPopup::~SetupAdvFollowRetargetPopup() {
}