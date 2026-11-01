#include "SetupTouchTogglePopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "CCTextInputNode.h"
#include "InfoAlertButton.h"

SetupTouchTogglePopup* SetupTouchTogglePopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupTouchTogglePopup* instance = new SetupTouchTogglePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupTouchTogglePopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_groupID = -1;
    m_toggleMode = -1;
    m_playerMode = -1;
    m_holdMode = true;
    m_dualMode = true;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!obj->m_touchHoldMode)
            m_holdMode = false;
        if (!obj->m_isDualMode)
            m_dualMode = false;

        if (m_groupID == -1)
            m_groupID = obj->m_targetGroupID;
        else if (m_groupID != obj->m_targetGroupID)
            m_groupID = 0;

        if (m_toggleMode == -1)
            m_toggleMode = obj->m_touchToggleMode;
        else if (m_toggleMode != obj->m_touchToggleMode)
            m_toggleMode = 0;

        if (m_playerMode == -1)
            m_playerMode = obj->m_touchPlayerMode;
        else if (m_playerMode != obj->m_touchPlayerMode)
            m_playerMode = 0;
    }
}

bool SetupTouchTogglePopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 320.0f, 260.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(titleLabel);
    titleLabel->setScale(0.5f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 16.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupTouchTogglePopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "<cg>Toggles</c> a group of objects when the player clicks.\n"
        "It also functions as a Spawn trigger.\n"
        "Alternates between toggling on and off for each click by default. Use <cy>Toggle On or Off</c> to limit to either.\n"
        "<cl>Hold Mode</c> toggles on and off for holding and releasing instead of clicking.\n"
        "<cp>Dual Mode</c> blocks player 2's clicks.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    cocos2d::CCPoint groupIDPos(winSize.width * 0.5f - 50.0f, winSize.height * 0.5f + 45.0f);

    cocos2d::CCLabelBMFont* groupIDLabel = cocos2d::CCLabelBMFont::create("Group ID", "bigFont.fnt");
    addChild(groupIDLabel);
    groupIDLabel->setScale(0.7f);
    groupIDLabel->setPosition(groupIDPos + cocos2d::CCPoint(0.0f, 30.0f));

    cocos2d::extension::CCScale9Sprite* groupIDBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    groupIDBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    groupIDBg->setOpacity(100);
    groupIDBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(groupIDBg, -1);
    groupIDBg->setPosition(groupIDPos);

    m_groupIDInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_groupIDInput, 20);
    m_groupIDInput->setPosition(groupIDPos);
    m_groupIDInput->setMaxLabelScale(0.6f);
    m_groupIDInput->setMaxLabelWidth(200.0f);
    m_groupIDInput->m_textField->setVisible(false);
    m_groupIDInput->m_maxLabelLength = 6;
    m_groupIDInput->setAllowedChars("0123456789");
    m_groupIDInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_groupIDInput->setLabelPlaceholderScale(0.5f);
    m_groupIDInput->m_delegate = this;
    m_groupIDInput->m_numberInput = true;
    m_groupIDInput->setTag(3);

    cocos2d::CCPoint inputLocalPos = m_buttonMenu->convertToNodeSpace(groupIDPos);

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupTouchTogglePopup::onTargetIDArrow));
    m_buttonMenu->addChild(leftItem);
    leftItem->setTag(1);
    leftItem->setPosition(inputLocalPos + cocos2d::CCPoint(-50.0f, 0.0f));
    leftItem->setSizeMult(2.0f);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupTouchTogglePopup::onTargetIDArrow));
    m_buttonMenu->addChild(rightItem);
    rightItem->setTag(2);
    rightItem->setPosition(inputLocalPos + cocos2d::CCPoint(50.0f, 0.0f));
    rightItem->setSizeMult(2.0f);

    updateTextInputLabel();

    createMultiTriggerItemsDefaultVertical();

    createToggleButton("Hold Mode", menu_selector(SetupTouchTogglePopup::onHoldMode), m_holdMode,
        cocos2d::CCPoint(winSize.width * 0.5f + 50.0f, winSize.height * 0.5f + 45.0f));

    cocos2d::CCPoint toggleBase(
        m_width * 0.5f + winSize.width * 0.5f - 70.0f,
        60.0f - (m_height * 0.5f - winSize.height * 0.5f));

    m_toggleOnToggler = createToggleButton("Toggle\nOn", menu_selector(SetupTouchTogglePopup::onTouchMode),
        m_toggleMode == 1, toggleBase);
    m_toggleOnToggler->m_notClickable = true;
    m_toggleOnToggler->setTag(1);

    m_toggleOffToggler = createToggleButton("Toggle\nOff", menu_selector(SetupTouchTogglePopup::onTouchMode),
        m_toggleMode == 2, toggleBase + cocos2d::CCPoint(0.0f, -35.0f));
    m_toggleOffToggler->m_notClickable = true;
    m_toggleOffToggler->setTag(2);

    createToggleButton("Dual\nMode", menu_selector(SetupTouchTogglePopup::onDualTouchMode), m_dualMode,
        cocos2d::CCPoint(m_width * 0.5f + winSize.width * 0.5f - 60.0f,
                         m_height * 0.5f + winSize.height * 0.5f - 20.0f));

    cocos2d::CCPoint controlBase(winSize.width * 0.5f - 50.0f, winSize.height * 0.5f - 20.0f);

    m_p1OnlyToggler = createToggleButton("P1\nOnly", menu_selector(SetupTouchTogglePopup::onControlMode),
        m_playerMode == 1, controlBase);
    m_p1OnlyToggler->m_notClickable = true;
    m_p1OnlyToggler->setTag(1);

    m_p2OnlyToggler = createToggleButton("P2\nOnly", menu_selector(SetupTouchTogglePopup::onControlMode),
        m_playerMode == 2, controlBase + cocos2d::CCPoint(100.0f, 0.0f));
    m_p2OnlyToggler->m_notClickable = true;
    m_p2OnlyToggler->setTag(2);

    m_disableTextDelegate = false;

    return true;
}

void SetupTouchTogglePopup::onClose(cocos2d::CCObject* sender)
{
    m_groupIDInput->m_delegate = nullptr;
    m_groupIDInput->onClickTrackNode(false);
    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    SetupTriggerPopup::onClose(sender);
}

void SetupTouchTogglePopup::onControlMode(cocos2d::CCObject* sender)
{
    m_playerMode = sender->getTag();

    if (m_playerMode == m_playerMode)
        m_playerMode = 0;

    if (m_gameObject)
    {
        m_gameObject->m_touchPlayerMode = m_playerMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_touchPlayerMode = m_playerMode;
        }
    }

    m_p1OnlyToggler->toggle(m_playerMode == 1);
    m_p2OnlyToggler->toggle(m_playerMode == 2);
}

void SetupTouchTogglePopup::onDualTouchMode(cocos2d::CCObject* sender)
{
    m_dualMode = !m_dualMode;

    if (m_gameObject)
    {
        m_gameObject->m_isDualMode = m_dualMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isDualMode = m_dualMode;
        }
    }
}

void SetupTouchTogglePopup::onHoldMode(cocos2d::CCObject* sender)
{
    m_holdMode = !m_holdMode;

    if (m_gameObject)
    {
        m_gameObject->m_touchHoldMode = m_holdMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_touchHoldMode = m_holdMode;
        }
    }
}

void SetupTouchTogglePopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_groupID -= 1;
    else
        m_groupID += 1;

    updateTargetID();
    updateEditorLabel();
    updateTextInputLabel();
}

void SetupTouchTogglePopup::onTouchMode(cocos2d::CCObject* sender)
{
    m_toggleMode = (sender->getTag() == 1) ? 1 : 2;

    if (m_toggleMode == m_toggleMode)
        m_toggleMode = 0;

    if (m_gameObject)
    {
        m_gameObject->m_touchToggleMode = m_toggleMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_touchToggleMode = m_toggleMode;
        }
    }

    m_toggleOnToggler->toggle(m_toggleMode == 1);
    m_toggleOffToggler->toggle(m_toggleMode == 2);
}

void SetupTouchTogglePopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate || node->getTag() != 3)
        return;

    m_groupID = atoi(node->getString().c_str());
    updateTargetID();
    updateEditorLabel();
}

void SetupTouchTogglePopup::updateTargetID()
{
    if (m_groupID < 0)
        m_groupID = 0;
    if (m_groupID > 9999)
        m_groupID = 9999;

    if (m_gameObject)
    {
        m_gameObject->setTargetID(m_groupID);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->setTargetID(m_groupID);
        }
    }
}

void SetupTouchTogglePopup::updateTextInputLabel()
{
    m_disableTextDelegate = true;

    m_groupIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_groupID)->getCString());

    m_disableTextDelegate = false;
}

SetupTouchTogglePopup::~SetupTouchTogglePopup()
{
}