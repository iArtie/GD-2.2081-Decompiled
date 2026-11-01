#include "Popups/Setup/SetupAnimationPopup.h"
#include "Objects/EffectGameObject.h"
#include "Objects/AnimatedGameObject.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCTextInputNode.h"
#include "UI/InfoAlertButton.h"

SetupAnimationPopup* SetupAnimationPopup::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    SetupAnimationPopup* popup = new SetupAnimationPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAnimationPopup::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 240.0f, 1)) {
        return false;
    }

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Animation", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        m_height * 0.5f + winSize.height * 0.5f - 18.0f));

    ButtonSprite* okSprite = ButtonSprite::create(
        "OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okButton = CCMenuItemSpriteExtra::create(
        okSprite, nullptr, menu_selector(SetupAnimationPopup::onClose));
    m_buttonMenu->addChild(okButton);

    std::string helpText =
        "<cg>Animates</c> certain objects. Can also be used to activate animate on trigger objects.\n"
        "The <cy>Animation ID</c> specifies the animation:";

    helpText += "\n<cg>Big Beast: </c>";
    for (int i = 0; i < 10; i++) {
        std::string animName = AnimatedGameObject::animationForID(i, 918);
        if (!animName.empty()) {
            if (i > 0) helpText += ", ";
            helpText += cocos2d::CCString::createWithFormat("<cy>%i:</c> %s", i, animName.c_str())->getCString();
        }
    }

    helpText += "\n\n<cg>Bat: </c>";
    for (int i = 0; i < 10; i++) {
        std::string animName = AnimatedGameObject::animationForID(i, 1584);
        if (!animName.empty()) {
            if (i > 0) helpText += ", ";
            helpText += cocos2d::CCString::createWithFormat("<cy>%i:</c> %s", i, animName.c_str())->getCString();
        }
    }

    helpText += "\n\n<cg>Spikeball: </c>";
    for (int i = 0; i < 10; i++) {
        std::string animName = AnimatedGameObject::animationForID(i, 2012);
        if (!animName.empty()) {
            if (i > 0) helpText += ", ";
            helpText += cocos2d::CCString::createWithFormat("<cy>%i:</c> %s", i, animName.c_str())->getCString();
        }
    }

    InfoAlertButton* helpButton = InfoAlertButton::create("Help", helpText, 1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(
            18.0f - (m_width * 0.5f - winSize.width * 0.5f),
            m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    for (int i = 0; i < 2; i++) {
        std::string label = i ? "Animation ID" : "Group ID";
        cocos2d::CCPoint inputPos;
        if (i) {
            inputPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 25.0f);
        } else {
            inputPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 40.0f);
        }

        cocos2d::CCLabelBMFont* fieldLabel = cocos2d::CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(fieldLabel);
        fieldLabel->setScale(0.65f);
        fieldLabel->setPosition(inputPos + cocos2d::CCPoint(0.0f, 30.0f));

        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png", cocos2d::CCRect(0, 0, 40, 40));
        bg->setOpacity(100);
        bg->setColor(ccc3(0, 0, 0));
        bg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(inputPos);

        CCTextInputNode* input = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
        m_mainLayer->addChild(input, 20);
        input->setPosition(inputPos);
        input->setMaxLabelScale(0.6f);
        input->setMaxLabelWidth(200.0f);
        input->m_textField->setVisible(false);
        input->m_maxLabelLength = 6;
        input->setAllowedChars("0123456789");
        input->setLabelPlaceholderColor(ccc3(150, 150, 150));
        input->setLabelPlaceholderScale(0.5f);
        input->m_delegate = this;
        input->m_numberInput = true;
        input->setTag(i);

        cocos2d::CCPoint menuPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, inputPos);

        cocos2d::CCSprite* leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        leftSprite->setScale(1.1f);
        CCMenuItemSpriteExtra* leftButton = CCMenuItemSpriteExtra::create(
            leftSprite, nullptr, i ? menu_selector(SetupAnimationPopup::onAnimationIDArrow) : menu_selector(SetupAnimationPopup::onTargetIDArrow));
        m_buttonMenu->addChild(leftButton);
        leftButton->setTag(1);
        leftButton->setPosition(menuPos + cocos2d::CCPoint(-50.0f, 0.0f));
        leftButton->setSizeMult(2.0f);

        cocos2d::CCSprite* rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        rightSprite->setScale(1.1f);
        CCMenuItemSpriteExtra* rightButton = CCMenuItemSpriteExtra::create(
            rightSprite, nullptr, i ? menu_selector(SetupAnimationPopup::onAnimationIDArrow) : menu_selector(SetupAnimationPopup::onTargetIDArrow));
        m_buttonMenu->addChild(rightButton);
        rightButton->setTag(2);
        rightButton->setPosition(menuPos + cocos2d::CCPoint(50.0f, 0.0f));
        rightButton->setSizeMult(2.0f);

        if (i) {
            m_animationIDInput = input;
        } else {
            m_targetIDInput = input;
        }
    }

    updateTextInputLabel();
    updateAnimationTextInputLabel();
    SetupTriggerPopup::createMultiTriggerItemsDefault();
    m_disableTextDelegate = false;

    return true;
}

void SetupAnimationPopup::determineStartValues() {
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject) {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects && m_gameObjects->count() > 0) {
        m_targetID = -1;
        m_animationID = -1;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);

            int targetID = ((GameObject*)obj)->m_uOrderOfArrival;
            if (m_targetID == -1) {
                m_targetID = targetID;
            } else if (m_targetID != targetID) {
                m_targetID = 0;
            }

            int animID = ((GameObject*)obj)->m_nLuaID;
            if (m_animationID == -1) {
                m_animationID = animID;
            } else if (m_animationID != animID) {
                m_animationID = 0;
            }
        }
    }
}

void SetupAnimationPopup::onAnimationIDArrow(cocos2d::CCObject* sender) {
    if (sender->getTag() == 1) {
        m_animationID--;
    } else {
        m_animationID++;
    }
    updateAnimationID();
    updateAnimationTextInputLabel();
}

void SetupAnimationPopup::onClose(cocos2d::CCObject* sender) {
    m_targetIDInput->m_delegate = nullptr;
    m_targetIDInput->onClickTrackNode(false);
    m_animationIDInput->m_delegate = nullptr;
    m_animationIDInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupAnimationPopup::onTargetIDArrow(cocos2d::CCObject* sender) {
    if (sender->getTag() == 1) {
        m_targetID--;
    } else {
        m_targetID++;
    }
    updateTargetID();
    SetupTriggerPopup::updateEditorLabel();
    updateTextInputLabel();
}

void SetupAnimationPopup::textChanged(CCTextInputNode* node) {
    if (m_disableTextDelegate) {
        return;
    }

    if (node->getTag()) {
        m_animationID = atoi(node->getString().c_str());
        updateAnimationID();
    } else {
        m_targetID = atoi(node->getString().c_str());
        updateTargetID();
        SetupTriggerPopup::updateEditorLabel();
    }
}

void SetupAnimationPopup::updateAnimationID() {
    if (m_animationID < 0) m_animationID = 0;
    if (m_animationID > 999) m_animationID = 999;

    if (m_gameObject) {
        m_gameObject->m_animationID = m_animationID;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            ((GameObject*)obj)->m_nLuaID = m_animationID;
        }
    }
}

void SetupAnimationPopup::updateAnimationTextInputLabel() {
    m_disableTextDelegate = true;
    m_animationIDInput->setString(cocos2d::CCString::createWithFormat("%i", m_animationID)->getCString());
    m_disableTextDelegate = false;
}

void SetupAnimationPopup::updateTargetID() {
    if (m_targetID < 0) m_targetID = 0;
    if (m_targetID > 9999) m_targetID = 9999;

    if (m_gameObject) {
        m_gameObject->setTargetID(m_targetID);
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->setTargetID(m_targetID);
        }
    }
}

void SetupAnimationPopup::updateTextInputLabel() {
    m_disableTextDelegate = true;
    m_targetIDInput->setString(cocos2d::CCString::createWithFormat("%i", m_targetID)->getCString());
    m_disableTextDelegate = false;
}

SetupAnimationPopup::~SetupAnimationPopup() {
}