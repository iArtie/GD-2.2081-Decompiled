#include "Popups/Setup/SetupCountTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"
#include "Utils/GameToolbox.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Cocos/CCTextInputNode.h"
#include "Managers/GameManager.h"
#include "Layers/LevelEditorLayer.h"

SetupCountTriggerPopup* SetupCountTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupCountTriggerPopup* instance = new SetupCountTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupCountTriggerPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_activateGroup = true;
    m_multiActivate = true;
    m_itemID = -1;
    m_targetID = -1;
    m_targetCount = 99999;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!obj->m_activateGroup)
            m_activateGroup = false;
        if (!obj->m_multiActivate)
            m_multiActivate = false;

        if (m_itemID == -1)
            m_itemID = obj->m_itemID;
        else if (m_itemID != obj->m_itemID)
            m_itemID = 0;

        if (m_targetID == -1)
            m_targetID = obj->m_targetGroupID;
        else if (m_targetID != obj->m_targetGroupID)
            m_targetID = 0;

        if (m_targetCount == 99999)
            m_targetCount = obj->m_targetCount;
        else if (m_targetCount != obj->m_targetCount)
            m_targetCount = 0;
    }
}

bool SetupCountTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
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
        okButton, nullptr, menu_selector(SetupCountTriggerPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Disable objects using <cg>Target ID</c> when an <cy>Item ID</c> reaches a certain <cl>Target Count</c>. The conditio"
        "n is checked constantly.\n"
        "<cp>Activate Group</c> makes the trigger enable and spawn the group instead.\n"
        "<cr>Multi Activate</c> allows the trigger to activate more than once.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    for (int i = 0; i < 3; i++)
    {
        std::string labelText;
        cocos2d::CCPoint basePos;
        SEL_MenuHandler arrowHandler = nullptr;
        int inputTag = 0;

        if (i == 0)
        {
            labelText = "Item ID";
            basePos = cocos2d::CCPoint(winSize.width * 0.5f - 70.0f, winSize.height * 0.5f + 40.0f);
            arrowHandler = menu_selector(SetupCountTriggerPopup::onItemIDArrow);
            inputTag = 0;
        }
        else if (i == 1)
        {
            labelText = "Target Count";
            basePos = cocos2d::CCPoint(winSize.width * 0.5f - 70.0f, winSize.height * 0.5f - 25.0f);
            arrowHandler = menu_selector(SetupCountTriggerPopup::onTargetCountArrow);
            inputTag = 1;
        }
        else
        {
            labelText = "Target ID";
            basePos = cocos2d::CCPoint(winSize.width * 0.5f + 70.0f, winSize.height * 0.5f + 40.0f);
            arrowHandler = menu_selector(SetupCountTriggerPopup::onTargetIDArrow);
            inputTag = 2;
        }

        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(labelText.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(label);
        label->setScale(0.65f);
        label->setPosition(basePos + cocos2d::CCPoint(0.0f, 30.0f));

        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
        bg->setColor(cocos2d::ccColor3B(0, 0, 0));
        bg->setOpacity(100);
        bg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(basePos);

        CCTextInputNode* input = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
        m_mainLayer->addChild(input, 20);
        input->setPosition(basePos);
        input->setMaxLabelScale(0.6f);
        input->setMaxLabelWidth(200.0f);
        input->m_textField->setVisible(false);
        input->m_maxLabelLength = 6;
        input->setAllowedChars(i == 1 ? "0123456789-" : "0123456789");
        input->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
        input->setLabelPlaceholderScale(0.5f);
        input->m_delegate = this;
        input->m_numberInput = true;
        input->setTag(inputTag);

        cocos2d::CCPoint inputPos = m_buttonMenu->convertToNodeSpace(basePos);

        cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        leftArrow->setScale(1.1f);
        CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(leftArrow, nullptr, arrowHandler);
        m_buttonMenu->addChild(leftItem);
        leftItem->setTag(1);
        leftItem->setPosition(inputPos + cocos2d::CCPoint(-50.0f, 0.0f));
        leftItem->setSizeMult(2.0f);

        cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        rightArrow->setScale(1.1f);
        CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(rightArrow, nullptr, arrowHandler);
        m_buttonMenu->addChild(rightItem);
        rightItem->setTag(2);
        rightItem->setPosition(inputPos + cocos2d::CCPoint(50.0f, 0.0f));
        rightItem->setSizeMult(2.0f);

        if (i == 0)
            m_itemIDInput = input;
        else if (i == 1)
            m_targetCountInput = input;
        else
            m_targetIDInput = input;
    }

    updateItemIDInputLabel();
    updateTargetIDInputLabel();
    updateCountTextInputLabel();

    cocos2d::CCPoint toggleBase(20.0f - (m_width * 0.5f - winSize.width * 0.5f),
                                60.0f - (m_height * 0.5f - winSize.height * 0.5f));

    m_touchToggle = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Touch\nTrigger", menu_selector(SetupTriggerPopup::onTouchTriggered), 0, m_touchTriggered,
        m_buttonMenu, toggleBase, this, m_mainLayer, 0.7f, 0.35f, 110.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    m_spawnToggle = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Spawn\nTrigger", menu_selector(SetupTriggerPopup::onSpawnedByTrigger), 0, m_spawnTriggered,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(0.0f, -35.0f), this, m_mainLayer, 0.7f, 0.35f, 110.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    GameToolbox::createToggleButton(
        "Multi\nTrigger", menu_selector(SetupTriggerPopup::onMultiTrigger), 0, m_multiTriggered,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(200.0f, 0.0f), this, m_mainLayer, 0.7f, 0.35f, 110.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, m_multiTriggerContainer);

    updateMultiTriggerBtn();

    createToggleButton("Multi\nActivate", menu_selector(SetupCountTriggerPopup::onMultiActivate), m_multiActivate,
        toggleBase + cocos2d::CCPoint(200.0f, -35.0f));

    createToggleButton("Activate\nGroup", menu_selector(SetupCountTriggerPopup::onEnableGroup), m_activateGroup,
        cocos2d::CCPoint(winSize.width * 0.5f + 50.0f, winSize.height * 0.5f - 20.0f));

    m_disableTextDelegate = false;

    return true;
}

void SetupCountTriggerPopup::onClose(cocos2d::CCObject* sender)
{
    m_itemIDInput->m_delegate = nullptr;
    m_itemIDInput->onClickTrackNode(false);
    m_targetIDInput->m_delegate = nullptr;
    m_targetIDInput->onClickTrackNode(false);
    m_targetCountInput->m_delegate = nullptr;
    m_targetCountInput->onClickTrackNode(false);
    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    SetupTriggerPopup::onClose(sender);
}

void SetupCountTriggerPopup::onEnableGroup(cocos2d::CCObject* sender)
{
    m_activateGroup = !m_activateGroup;

    if (m_gameObject)
    {
        m_gameObject->m_activateGroup = m_activateGroup;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_activateGroup = m_activateGroup;
        }
    }
}

void SetupCountTriggerPopup::onItemIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_itemID -= 1;
    else
        m_itemID += 1;

    updateItemID();
    updateEditorLabel();
    updateItemIDInputLabel();
}

void SetupCountTriggerPopup::onMultiActivate(cocos2d::CCObject* sender)
{
    m_multiActivate = !m_multiActivate;

    if (m_gameObject)
    {
        m_gameObject->m_multiActivate = m_multiActivate;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_multiActivate = m_multiActivate;
        }
    }
}

void SetupCountTriggerPopup::onTargetCountArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_targetCount -= 1;
    else
        m_targetCount += 1;

    updateTargetCount();
    updateCountTextInputLabel();
}

void SetupCountTriggerPopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_targetID -= 1;
    else
        m_targetID += 1;

    updateTargetID();
    updateTargetIDInputLabel();
}

void SetupCountTriggerPopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    if (node->getTag() == 1)
    {
        m_targetCount = atoi(node->getString().c_str());
        updateTargetCount();
    }
    else if (node->getTag() == 2)
    {
        m_targetID = atoi(node->getString().c_str());
        updateTargetID();
    }
    else
    {
        m_itemID = atoi(node->getString().c_str());
        updateItemID();
        updateEditorLabel();
    }
}

void SetupCountTriggerPopup::updateCountTextInputLabel()
{
    m_disableTextDelegate = true;
    m_targetCountInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_targetCount)->getCString());
    m_disableTextDelegate = false;
}

void SetupCountTriggerPopup::updateItemID()
{
    if (m_itemID < 0)
        m_itemID = 0;
    if (m_itemID > 9999)
        m_itemID = 9999;

    if (m_gameObject)
    {
        m_gameObject->m_itemID = m_itemID;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_itemID = m_itemID;
        }
    }
}

void SetupCountTriggerPopup::updateItemIDInputLabel()
{
    m_disableTextDelegate = true;
    m_itemIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_itemID)->getCString());
    m_disableTextDelegate = false;
}

void SetupCountTriggerPopup::updateTargetCount()
{
    if (m_gameObject)
    {
        m_gameObject->m_targetCount = m_targetCount;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_targetCount = m_targetCount;
        }
    }
}

void SetupCountTriggerPopup::updateTargetID()
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
    updateEditorLabel();
}

void SetupCountTriggerPopup::updateTargetIDInputLabel()
{
    m_disableTextDelegate = true;
    m_targetIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_targetID)->getCString());
    m_disableTextDelegate = false;
}

SetupCountTriggerPopup::~SetupCountTriggerPopup()
{
}