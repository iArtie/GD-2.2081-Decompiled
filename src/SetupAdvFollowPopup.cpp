#include "SetupAdvFollowPopup.h"
#include "AdvancedFollowTriggerObject.h"
#include "SelectPremadeLayer.h"
#include "CCTextInputNode.h"

SetupAdvFollowPopup* SetupAdvFollowPopup::create(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects) {
    SetupAdvFollowPopup* popup = new SetupAdvFollowPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAdvFollowPopup::init(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 260.0f, 3)) {
        return false;
    }

    m_gameObject = object;
    m_gameObjects = objects;

    return true;
}

void SetupAdvFollowPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    SetupTriggerPopup::updateValue(572, (float)tag);
    SetupTriggerPopup::updateCustomToggleTrigger(0, tag == 0);
    SetupTriggerPopup::updateCustomToggleTrigger(1, tag == 1);
    SetupTriggerPopup::updateCustomToggleTrigger(2, tag == 2);
}

void SetupAdvFollowPopup::onMode(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    updateMode(tag);
}

void SetupAdvFollowPopup::onPremade(cocos2d::CCObject* sender) {
    SelectPremadeLayer* layer = SelectPremadeLayer::create();
    layer->m_delegate = this;
    layer->show();
}

void SetupAdvFollowPopup::selectPremadeClosed(SelectPremadeLayer* layer, int type) {
    if (type == 0) {
        SetupTriggerPopup::updateValue(339, 1.0f);
        SetupTriggerPopup::updateValue(292, 0.0f);
        SetupTriggerPopup::updateValue(293, 0.0f);
        SetupTriggerPopup::updateValue(298, 5.0f);
        SetupTriggerPopup::updateValue(299, 0.0f);
        SetupTriggerPopup::updateValue(334, 0.5f);
        SetupTriggerPopup::updateValue(335, 0.0f);
        SetupTriggerPopup::updateValue(316, 1.4f);
        SetupTriggerPopup::updateValue(317, 0.0f);
        SetupTriggerPopup::updateValue(337, 1.0f);
        SetupTriggerPopup::updateValue(318, 5.0f);
        SetupTriggerPopup::updateValue(319, 0.0f);
        SetupTriggerPopup::updateValue(322, 0.0f);
        SetupTriggerPopup::updateValue(323, 0.0f);
        SetupTriggerPopup::updateValue(338, 1.0f);
        SetupTriggerPopup::updateValue(320, 0.25f);
        SetupTriggerPopup::updateValue(321, 0.0f);
        SetupTriggerPopup::updateValue(324, 2.0f);
        SetupTriggerPopup::updateValue(325, 0.0f);
        SetupTriggerPopup::updateValue(326, 0.25f);
        SetupTriggerPopup::updateValue(327, 0.0f);
        SetupTriggerPopup::updateValue(328, 70.0f);
        SetupTriggerPopup::updateValue(329, 0.0f);
        SetupTriggerPopup::updateValue(330, 0.2f);
        SetupTriggerPopup::updateValue(331, 0.0f);
        SetupTriggerPopup::updateValue(332, 2.0f);
        SetupTriggerPopup::updateValue(333, 0.0f);
        updateMode(2);
    } else if (type == 1) {
        SetupTriggerPopup::updateValue(292, 0.0f);
        SetupTriggerPopup::updateValue(293, 0.0f);
        SetupTriggerPopup::updateValue(298, 0.6f);
        SetupTriggerPopup::updateValue(299, 0.0f);
        SetupTriggerPopup::updateValue(334, 0.2f);
        SetupTriggerPopup::updateValue(335, 0.0f);
        SetupTriggerPopup::updateValue(316, 5.0f);
        SetupTriggerPopup::updateValue(317, 0.0f);
        SetupTriggerPopup::updateValue(337, 0.0f);
        SetupTriggerPopup::updateValue(318, 0.0f);
        SetupTriggerPopup::updateValue(319, 0.0f);
        SetupTriggerPopup::updateValue(322, 0.0f);
        SetupTriggerPopup::updateValue(323, 0.0f);
        SetupTriggerPopup::updateValue(338, 0.0f);
        SetupTriggerPopup::updateValue(320, 0.0f);
        SetupTriggerPopup::updateValue(321, 0.0f);
        SetupTriggerPopup::updateValue(324, 0.0f);
        SetupTriggerPopup::updateValue(325, 0.0f);
        SetupTriggerPopup::updateValue(326, 0.0f);
        SetupTriggerPopup::updateValue(327, 0.0f);
        SetupTriggerPopup::updateValue(328, 0.0f);
        SetupTriggerPopup::updateValue(329, 0.0f);
        SetupTriggerPopup::updateValue(330, 0.0f);
        SetupTriggerPopup::updateValue(331, 0.0f);
        SetupTriggerPopup::updateValue(332, 0.0f);
        SetupTriggerPopup::updateValue(333, 0.0f);
        updateMode(2);
    }
}

void SetupAdvFollowPopup::updateDefaultTriggerValues() {
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

void SetupAdvFollowPopup::updateMode(int mode) {
    SetupTriggerPopup::updateValue(367, (float)mode);

    cocos2d::CCNode* parent = getParent();
    int zOrder = getZOrder();

    onClose(nullptr);

    SetupAdvFollowPopup* newPopup = SetupAdvFollowPopup::create((AdvancedFollowTriggerObject*)m_gameObject, m_gameObjects);
    parent->addChild(newPopup, zOrder);
    newPopup->goToPage(1, false);
}

void SetupAdvFollowPopup::valueDidChange(int tag, float value) {
    if (tag > 201) {
        if (tag == 306) {
            if (value != 0.0f) {
                SetupTriggerPopup::updateValue(307, 0.0f);
            }
        } else if (tag == 307 && value != 0.0f) {
            SetupTriggerPopup::updateValue(306, 0.0f);
        }
        return;
    }

    if (tag < 200 && tag != 138) {
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

SetupAdvFollowPopup::~SetupAdvFollowPopup() {
}