#include "SetupCameraEdgePopup.h"
#include "SetupTriggerPopup.h"
#include "CameraTriggerGameObject.h"
#include "EffectGameObject.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "InfoAlertButton.h"
#include "CCTextInputNode.h"

SetupCameraEdgePopup* SetupCameraEdgePopup::create(CameraTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupCameraEdgePopup* instance = new SetupCameraEdgePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupCameraEdgePopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects && m_gameObjects->count() > 0)
    {
        m_targetID = -1;
        m_edgeDirection = -1;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

            if (m_targetID == -1)
                m_targetID = obj->m_targetGroupID;
            else if (m_targetID != obj->m_targetGroupID)
                m_targetID = 0;

            int direction = obj->m_edgeDirection;
            if (m_edgeDirection == -1)
                m_edgeDirection = direction;
            else if (m_edgeDirection != direction)
                m_edgeDirection = 0;
        }
    }
}

bool SetupCameraEdgePopup::init(CameraTriggerGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 240.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 18.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupCameraEdgePopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Makes one of the camera's <cg>edges</c> a specified target object.\n"
        "Use another trigger with <cy>Unlock</c> (Target ID 0) to reset.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f);

    cocos2d::CCLabelBMFont* idLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(idLabel);
    idLabel->setScale(0.65f);
    idLabel->setPosition(center + cocos2d::CCPoint(0.0f, 30.0f));

    cocos2d::extension::CCScale9Sprite* inputBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    inputBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    inputBg->setOpacity(100);
    inputBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(inputBg, -1);
    inputBg->setPosition(center);

    m_targetIDInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_targetIDInput, 20);
    m_targetIDInput->setPosition(center);
    m_targetIDInput->setMaxLabelScale(0.6f);
    m_targetIDInput->setMaxLabelWidth(200.0f);
    m_targetIDInput->m_textField->setVisible(false);
    m_targetIDInput->m_maxLabelLength = 6;
    m_targetIDInput->setAllowedChars("0123456789");
    m_targetIDInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_targetIDInput->setLabelPlaceholderScale(0.5f);
    m_targetIDInput->m_delegate = this;
    m_targetIDInput->m_numberInput = true;

    cocos2d::CCPoint inputPos = m_buttonMenu->convertToNodeSpace(center);

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupCameraEdgePopup::onTargetIDArrow));
    m_buttonMenu->addChild(leftItem);
    leftItem->setTag(1);
    leftItem->setPosition(inputPos + cocos2d::CCPoint(-50.0f, 0.0f));
    leftItem->setSizeMult(2.0f);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupCameraEdgePopup::onTargetIDArrow));
    m_buttonMenu->addChild(rightItem);
    rightItem->setTag(2);
    rightItem->setPosition(inputPos + cocos2d::CCPoint(50.0f, 0.0f));
    rightItem->setSizeMult(2.0f);

    updateTextInputLabel();

    m_directionToggles = cocos2d::CCArray::create();
    m_directionToggles->retain();

    cocos2d::CCPoint toggleBase(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f);

    CCMenuItemToggler* leftToggle = GameToolbox::createToggleButton(
        "Left", menu_selector(SetupCameraEdgePopup::onCameraEdge), 0, m_edgeDirection == 1,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(-75.0f, 0.0f), this, m_mainLayer,
        0.6f, 0.35f, 0.0f, cocos2d::CCPoint(0.0f, 2.0f), "bigFont.fnt", false, 0, false);
    leftToggle->setTag(1);
    leftToggle->m_notClickable = true;

    CCMenuItemToggler* rightToggle = GameToolbox::createToggleButton(
        "Right", menu_selector(SetupCameraEdgePopup::onCameraEdge), 0, m_edgeDirection == 2,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(-25.0f, 0.0f), this, m_mainLayer,
        0.6f, 0.35f, 0.0f, cocos2d::CCPoint(0.0f, 2.0f), "bigFont.fnt", true, 0, false);
    rightToggle->setTag(2);
    rightToggle->m_notClickable = true;

    CCMenuItemToggler* upToggle = GameToolbox::createToggleButton(
        "Up", menu_selector(SetupCameraEdgePopup::onCameraEdge), 0, m_edgeDirection == 3,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(25.0f, 0.0f), this, m_mainLayer,
        0.6f, 0.35f, 0.0f, cocos2d::CCPoint(0.0f, 2.0f), "bigFont.fnt", true, 0, false);
    upToggle->setTag(3);
    upToggle->m_notClickable = true;

    CCMenuItemToggler* downToggle = GameToolbox::createToggleButton(
        "Down", menu_selector(SetupCameraEdgePopup::onCameraEdge), 0, m_edgeDirection == 4,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(75.0f, 0.0f), this, m_mainLayer,
        0.6f, 0.35f, 0.0f, cocos2d::CCPoint(0.0f, 2.0f), "bigFont.fnt", true, 0, false);
    downToggle->setTag(4);
    downToggle->m_notClickable = true;

    m_directionToggles->addObject(leftToggle);
    m_directionToggles->addObject(rightToggle);
    m_directionToggles->addObject(upToggle);
    m_directionToggles->addObject(downToggle);

    ButtonSprite* unlockButton = ButtonSprite::create(
        "Unlock", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_05.png", 30.0f);
    CCMenuItemSpriteExtra* unlockItem = CCMenuItemSpriteExtra::create(
        unlockButton, nullptr, menu_selector(SetupCameraEdgePopup::onUnlockEdge));
    m_buttonMenu->addChild(unlockItem);
    unlockItem->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 100.0f, winSize.height * 0.5f - 20.0f)));

    createMultiTriggerItemsDefault();

    m_disableTextDelegate = false;

    return true;
}

void SetupCameraEdgePopup::onCameraEdge(cocos2d::CCObject* sender)
{
    m_edgeDirection = sender->getTag();

    ((CCMenuItemToggler*)m_directionToggles->objectAtIndex(0))->toggle(m_edgeDirection == 1);
    ((CCMenuItemToggler*)m_directionToggles->objectAtIndex(1))->toggle(m_edgeDirection == 2);
    ((CCMenuItemToggler*)m_directionToggles->objectAtIndex(2))->toggle(m_edgeDirection == 3);
    ((CCMenuItemToggler*)m_directionToggles->objectAtIndex(3))->toggle(m_edgeDirection == 4);

    if (m_gameObject)
    {
        m_gameObject->m_edgeDirection = m_edgeDirection;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_edgeDirection = m_edgeDirection;
        }
    }
}

void SetupCameraEdgePopup::onClose(cocos2d::CCObject* sender)
{
    m_targetIDInput->m_delegate = nullptr;
    m_targetIDInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupCameraEdgePopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_targetID -= 1;
    else
        m_targetID += 1;

    updateTargetID();
    updateEditorLabel();
    updateTextInputLabel();
}

void SetupCameraEdgePopup::onUnlockEdge(cocos2d::CCObject* sender)
{
    m_targetID = 0;
    updateTargetID();
    updateTextInputLabel();
    updateEditorLabel();
}

void SetupCameraEdgePopup::textChanged(CCTextInputNode* node)
{
    if (!m_disableTextDelegate)
    {
        std::string text = node->getString();
        m_targetID = atoi(text.c_str());
        updateTargetID();
        updateEditorLabel();
    }
}

void SetupCameraEdgePopup::updateTargetID()
{
    if (m_targetID < 0)
        m_targetID = 0;
    if (m_targetID > 9999)
        m_targetID = 9999;

    if (m_gameObject)
    {
        m_gameObject->setTargetID(m_targetID);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->setTargetID(m_targetID);
        }
    }
}

void SetupCameraEdgePopup::updateTextInputLabel()
{
    m_disableTextDelegate = true;

    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", m_targetID);
    m_targetIDInput->setString(str->getCString());

    m_disableTextDelegate = false;
}

SetupCameraEdgePopup::~SetupCameraEdgePopup()
{
}