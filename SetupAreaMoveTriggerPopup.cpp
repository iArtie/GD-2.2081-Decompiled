#include "SetupAreaMoveTriggerPopup.h"
#include "EnterEffectObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "CCMenuItemToggler.h"

SetupAreaMoveTriggerPopup* SetupAreaMoveTriggerPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects) {
    SetupAreaMoveTriggerPopup* popup = new SetupAreaMoveTriggerPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAreaMoveTriggerPopup::init(EnterEffectObject* object, cocos2d::CCArray* objects) {
    if (!SetupAreaTriggerPopup::init(object, objects, 400.0f, 300.0f, 2)) {
        return false;
    }

    addAreaDefaultControls(object ? object->m_objectID : 0);

    return true;
}

void SetupAreaMoveTriggerPopup::addAreaDefaultControls(int objectID) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint basePos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f);

    SetupTriggerPopup::createValueControlAdvanced(
        252, "OffsetY",
        cocos2d::CCPoint(0.65f, 0.0f), 100.0f, true,
        basePos + cocos2d::CCPoint(-75.0f, 0.0f),
        InputValueType::Float, 2, 6,
        false, true, 0, 1, GJInputStyle::Normal, 1, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        253, "+-",
        cocos2d::CCPoint(0.65f, 0.0f), 100.0f, true,
        basePos + cocos2d::CCPoint(75.0f, 0.0f),
        InputValueType::Float, 2, 6,
        false, true, 0, 1, GJInputStyle::Normal, 1, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        225, "EffectID:",
        cocos2d::CCPoint(0.65f, 0.0f), 100.0f, true,
        basePos + cocos2d::CCPoint(85.0f, -50.0f),
        InputValueType::Float, 3, 6,
        true, true, 1, 1, GoldLabel, 0, 2);

    cocos2d::CCSprite* plusSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    plusSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* plusButton = CCMenuItemSpriteExtra::create(
        plusSprite, nullptr, menu_selector(SetupAreaMoveTriggerPopup::onNextFreeEffectID));
    m_buttonMenu->addChild(plusButton);
    plusButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        basePos + cocos2d::CCPoint(145.0f, -50.0f)));
    SetupTriggerPopup::addObjectToPage(plusButton, 1);

    SetupTriggerPopup::createValueControlAdvanced(
        51, "Target Group ID:",
        cocos2d::CCPoint(0.65f, 0.0f), 100.0f, true,
        basePos + cocos2d::CCPoint(85.0f, -100.0f),
        InputValueType::Float, 3, 6,
        true, true, 1, 1, GoldLabel, 0, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        71, "Center Group ID:",
        cocos2d::CCPoint(0.65f, 0.0f), 100.0f, true,
        basePos + cocos2d::CCPoint(85.0f, -150.0f),
        InputValueType::Float, 3, 6,
        true, true, 1, 1, GoldLabel, 0, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        341, "Priority:",
        cocos2d::CCPoint(0.65f, 0.0f), 100.0f, true,
        basePos + cocos2d::CCPoint(85.0f, -200.0f),
        InputValueType::Float, 2, 6,
        true, true, 1, 1, GoldLabel, 0, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        263, "ModFront",
        cocos2d::CCPoint(0.65f, 0.0f), 10.0f, true,
        basePos + cocos2d::CCPoint(-75.0f, -25.0f),
        InputValueType::Float, 1, 6,
        false, true, 0, 1, GoldLabel, 1, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        264, "ModBack",
        cocos2d::CCPoint(0.65f, 0.0f), 10.0f, true,
        basePos + cocos2d::CCPoint(-75.0f, -50.0f),
        InputValueType::Float, 1, 6,
        false, true, 0, 1, GoldLabel, 1, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        282, "Deadzone",
        cocos2d::CCPoint(0.65f, 0.0f), 1.0f, true,
        basePos + cocos2d::CCPoint(-75.0f, -75.0f),
        InputValueType::Float, 1, 6,
        false, true, 0, 1, GoldLabel, 1, 2);

    m_targetButtons = cocos2d::CCArray::create();
    m_targetButtons->retain();

    cocos2d::CCPoint targetBase = cocos2d::CCPoint(winSize.width * 0.5f + 170.0f, winSize.height * 0.5f + 40.0f);

    for (int i = 1; i < 12; i++) {
        int targetValue = -i;
        std::string targetName = GameToolbox::targetKeyToString(targetValue);

        cocos2d::CCArray* buttonObjects = cocos2d::CCArray::create();

        cocos2d::CCNode* toggleButton = GameToolbox::createToggleButton(
            targetName.c_str(),
            menu_selector(SetupAreaMoveTriggerPopup::onSpecialTarget),
            false, false, m_buttonMenu,
            targetBase + cocos2d::CCPoint(0.0f, (float)i * -23.0f),
            this, m_mainLayer,
            0.6f, 0.35f, 110.0f,
            cocos2d::CCPoint(6.0f, 0.0f),
            "bigFont.fnt", false, 0, buttonObjects);

        toggleButton->m_bNoGlow = true;
        toggleButton->setTag(targetValue);

        SetupTriggerPopup::addObjectsToPage(buttonObjects, 1);
        m_targetButtons->addObject(toggleButton);
    }

    cocos2d::CCPoint toggleBase = cocos2d::CCPoint(
        winSize.width * 0.5f - 10.0f,
        winSize.height * 0.5f - 80.0f);
    cocos2d::CCPoint infoBase = cocos2d::CCPoint(
        20.0f - (m_width * 0.5f - winSize.width * 0.5f),
        (20.0f - (m_height * 0.5f - winSize.height * 0.5f)) + 30.0f);

    SetupTriggerPopup::createToggleValueControl(
        280, "Ignore\nGParent",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        infoBase + cocos2d::CCPoint(90.0f, 0.0f),
        false, true, 0.0f);

    SetupTriggerPopup::createToggleValueControl(
        281, "Ignore\nLinked",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        infoBase,
        false, true, 0.0f);

    SetupTriggerPopup::createToggleValueControl(
        539, "DE\nAP",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        infoBase + cocos2d::CCPoint(180.0f, 20.0f),
        false, true, 0.0f);

    if (objectID >= 3006 && objectID <= 3008) {
        SetupTriggerPopup::createCustomEasingControls(
            "Easing", cocos2d::CCPoint(0.7f, 0.0f),
            basePos + cocos2d::CCPoint(-95.0f, -150.0f),
            242, 243, 0);

        SetupTriggerPopup::createToggleValueControl(
            261, "Ease\nOut",
            cocos2d::CCPoint(1.0f, 0.0f), true,
            cocos2d::CCPoint(winSize.width * 0.5f + 50.0f,
                             20.0f - (m_height * 0.5f - winSize.height * 0.5f)),
            false, false, 0.0f);

        SetupTriggerPopup::createCustomEasingControls(
            "Easing2", cocos2d::CCPoint(0.7f, 0.0f),
            basePos + cocos2d::CCPoint(95.0f, -150.0f),
            248, 249, 0);

        bool easeOut = (SetupTriggerPopup::getValue(261) != 0.0f);
        SetupTriggerPopup::toggleGroup(9, easeOut);
    }

    m_modeButtons = cocos2d::CCArray::create();
    m_modeButtons->retain();

    float currentMode = SetupTriggerPopup::getValue(262);
    float inboundFlag = SetupTriggerPopup::getValue(276) != 0.0f;
    float twoDirFlag = SetupTriggerPopup::getValue(283) != 0.0f;

    cocos2d::CCArray* pageContainer = SetupTriggerPopup::getPageContainer(1);

    for (int i = 0; i < 10; i++) {
        const char* frameName;
        float rotation = 0.0f;

        switch (i) {
        case 1: frameName = "edit_areaModeBtn05_001.png"; break;
        case 2: frameName = "edit_areaModeBtn01_001.png"; break;
        case 3: frameName = "edit_areaModeBtn02_001.png"; break;
        case 4: frameName = "edit_areaModeBtn03_001.png"; break;
        case 5: frameName = "edit_areaModeBtn03b_001.png"; break;
        case 6: frameName = "edit_areaModeBtn01_001.png"; rotation = 90.0f; break;
        case 7: frameName = "edit_areaModeBtn02_001.png"; rotation = 90.0f; break;
        case 8: frameName = "edit_areaModeBtn03b_001.png"; rotation = -90.0f; break;
        case 9: frameName = "edit_areaModeBtn03_001.png"; rotation = -90.0f; break;
        default: frameName = "edit_areaModeBtn04_001.png"; break;
        }

        cocos2d::CCSprite* modeSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        modeSprite->setRotation(rotation);

        int direction; bool inbound; bool twoDir;
        GameToolbox::getModeValues(i, &direction, &inbound, &twoDir);

        bool isSelected = (direction == (int)currentMode && inbound == (inboundFlag != 0.0f) && twoDir == (twoDirFlag != 0.0f));
        const char* bgFrame = isSelected ? "GJ_button_02.png" : "GJ_button_04.png";

        ButtonSprite* modeButton = ButtonSprite::create(
            modeSprite, 32.0f, 1.0f, bgFrame, true);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            modeButton, nullptr, menu_selector(SetupAreaMoveTriggerPopup::onMode));
        item->setTag(i);

        cocos2d::CCArray::addObject(pageContainer, item);
        cocos2d::CCArray::addObject(m_modeButtons, item);

        modeSprite->setScale(1.8f);
        modeSprite->setPosition(cocos2d::CCPoint(
            item->getContentSize().width * 0.5f,
            item->getContentSize().height * 0.5f));

        item->setScale(0.5f);
        item->m_baseScale = 0.5f;
        m_buttonMenu->addChild(item);

        float yOffset = (i > 4) ? -25.0f : 0.0f;
        item->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
            cocos2d::CCPoint(
                (50.0f - (m_width * 0.5f - winSize.width * 0.5f)) + (float)(i % 5) * 25.0f,
                yOffset + (winSize.height * 0.5f - 20.0f))));
    }
}

void SetupAreaMoveTriggerPopup::onMode(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int direction; bool inbound; bool twoDir;
    GameToolbox::getModeValues(tag, &direction, &inbound, &twoDir);

    SetupTriggerPopup::toggleGroup(99, direction == 0);
    SetupTriggerPopup::updateValue(262, (float)direction);
    SetupTriggerPopup::updateValue(276, (float)inbound);
    SetupTriggerPopup::updateValue(283, (float)twoDir);

    for (unsigned int i = 0; i < m_modeButtons->count(); i++) {
        CCMenuItemSpriteExtra* item = (CCMenuItemSpriteExtra*)m_modeButtons->objectAtIndex(i);
        ButtonSprite* button = (ButtonSprite*)item->getNormalImage();
        if ((int)i == sender->getTag()) {
            button->updateBGImage("GJ_button_02.png");
        } else {
            button->updateBGImage("GJ_button_04.png");
        }
    }
}

void SetupAreaMoveTriggerPopup::onNextFreeEffectID(cocos2d::CCObject* sender) {
    int nextID = LevelEditorLayer::getNextFreeAreaEffectID(
        GameManager::sharedState()->m_levelEditorLayer, nullptr);
    SetupTriggerPopup::updateValue(225, (float)nextID);
}

void SetupAreaMoveTriggerPopup::onSpecialTarget(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    float newValue = ((int)SetupTriggerPopup::getValue(538) == tag) ? 0.0f : (float)tag;
    SetupTriggerPopup::updateValue(538, newValue);
    updateEnterTargetIDState();
}

float SetupAreaMoveTriggerPopup::triggerSliderValueFromValue(int tag, float value) {
    float adjustedValue = shouldModifyValue(tag) ? value / 3.0f : value;
    return SetupTriggerPopup::triggerSliderValueFromValue(tag, adjustedValue);
}

float SetupAreaMoveTriggerPopup::triggerValueFromSliderValue(int tag, float value) {
    float result = SetupTriggerPopup::triggerValueFromSliderValue(tag, value);
    if (shouldModifyValue(tag)) {
        return floorf(result) * 3.0f;
    }
    return result;
}

void SetupAreaMoveTriggerPopup::updateDefaultTriggerValues() {
    SetupTriggerPopup::updateDefaultTriggerValues();
    updateEnterTargetIDState();
    SetupTriggerPopup::toggleGroup(99, SetupTriggerPopup::getValue(262) == 0.0f);
}

void SetupAreaMoveTriggerPopup::updateEnterTargetIDState() {
    int targetID = (int)SetupTriggerPopup::getValue(538);
    int effectiveID = targetID ? targetID : (int)SetupTriggerPopup::getValue(71);

    if (m_targetButtons) {
        for (unsigned int i = 0; i < m_targetButtons->count(); i++) {
            CCMenuItemToggler* toggler = (CCMenuItemToggler*)m_targetButtons->objectAtIndex(i);
            toggler->toggle(effectiveID == toggler->getTag());
        }
    }

    CCTextInputNode* inputNode = (CCTextInputNode*)cocos2d::CCDictionary::objectForKey(
        m_inputNodes, cocos2d::CCString::createWithFormat("%i", 71)->getCString());

    if (inputNode) {
        m_disableTextDelegate = true;
        std::string value;
        if (effectiveID < 0) {
            value = GameToolbox::targetKeyToString(effectiveID);
        } else {
            value = GameToolbox::intToString(effectiveID);
        }
        inputNode->setString(value.c_str());
        m_disableTextDelegate = false;
    }
}

void SetupAreaMoveTriggerPopup::updateInputNode(int tag, float value) {
    float adjustedValue = shouldModifyValue(tag) ? value / 3.0f : value;
    SetupTriggerPopup::updateInputNode(tag, adjustedValue);
}

void SetupAreaMoveTriggerPopup::updateInputValue(int tag, float* value) {
    if (shouldModifyValue(tag)) {
        *value *= 3.0f;
    }
}

SetupAreaMoveTriggerPopup::~SetupAreaMoveTriggerPopup() {
}