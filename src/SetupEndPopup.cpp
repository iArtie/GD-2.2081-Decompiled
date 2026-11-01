#include "SetupEndPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "InfoAlertButton.h"
#include "CCTextInputNode.h"

SetupEndPopup* SetupEndPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupEndPopup* instance = new SetupEndPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupEndPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_reversed = true;
    m_lockY = true;
    m_groupID = -1;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!obj->m_endReversed)
            m_reversed = false;
        if (!obj->m_lockToPlayerY)
            m_lockY = false;

        if (m_groupID == -1)
            m_groupID = obj->m_targetGroupID;
        else if (m_groupID != obj->m_targetGroupID)
            m_groupID = 0;
    }
}

bool SetupEndPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 240.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 18.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupEndPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Sets a specific object given by <cg>Group ID</c> to be the <cy>end</c> of the level when the game type is Classic."
        " This essentially spawns the endwall at a set location.\n"
        "Use <cl>Reversed</c> if gameplay is reverse at the end.\n"
        "<cp>Lock Y</c> locks the endwall to the ground instead of the group's Y position.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    cocos2d::CCPoint groupIDPos(winSize.width * 0.5f, winSize.height * 0.5f + 30.0f);

    cocos2d::CCLabelBMFont* groupIDLabel = cocos2d::CCLabelBMFont::create("Group ID:", "bigFont.fnt");
    addChild(groupIDLabel);
    groupIDLabel->setScale(0.7f);
    groupIDLabel->setPosition(groupIDPos + cocos2d::CCPoint(0.0f, 34.0f));

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

    cocos2d::CCPoint inputPos = m_buttonMenu->convertToNodeSpace(groupIDPos);

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupEndPopup::onTargetIDArrow));
    m_buttonMenu->addChild(leftItem);
    leftItem->setTag(1);
    leftItem->setPosition(inputPos + cocos2d::CCPoint(-50.0f, 0.0f));
    leftItem->setSizeMult(2.0f);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupEndPopup::onTargetIDArrow));
    m_buttonMenu->addChild(rightItem);
    rightItem->setTag(2);
    rightItem->setPosition(inputPos + cocos2d::CCPoint(50.0f, 0.0f));
    rightItem->setSizeMult(2.0f);

    updateTextInputLabel();

    cocos2d::CCPoint toggleBase(20.0f - (m_width * 0.5f - winSize.width * 0.5f),
                                60.0f - (m_height * 0.5f - winSize.height * 0.5f));

    if (object && object->getType() == GameObjectType::CustomRing)
    {
        createToggleButton("Multi\nActivate", menu_selector(SetupEndPopup::onMultiActivate), m_multiActivate,
            toggleBase + cocos2d::CCPoint(0.0f, -35.0f));
    }
    else
    {
        createMultiTriggerItemsDefault();
    }

    createToggleButton("Reversed", menu_selector(SetupEndPopup::onReversed), m_reversed,
        cocos2d::CCPoint(winSize.width * 0.5f - 50.0f, winSize.height * 0.5f - 20.0f));

    createToggleButton("Lock Y", menu_selector(SetupEndPopup::onLockY), m_lockY,
        cocos2d::CCPoint(winSize.width * 0.5f + 50.0f, winSize.height * 0.5f - 20.0f));

    m_disableTextDelegate = false;

    return true;
}

void SetupEndPopup::onClose(cocos2d::CCObject* sender)
{
    m_groupIDInput->m_delegate = nullptr;
    m_groupIDInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupEndPopup::onLockY(cocos2d::CCObject* sender)
{
    m_lockY = !m_lockY;

    if (m_gameObject)
    {
        m_gameObject->m_lockToPlayerY = m_lockY;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_lockToPlayerY = m_lockY;
        }
    }
}

void SetupEndPopup::onMultiActivate(cocos2d::CCObject* sender)
{
    m_multiActivate = !m_multiActivate;

    if (m_gameObject)
        m_gameObject->m_isMultiActivate = m_multiActivate;
}

void SetupEndPopup::onReversed(cocos2d::CCObject* sender)
{
    m_reversed = !m_reversed;

    if (m_gameObject)
    {
        m_gameObject->m_endReversed = m_reversed;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_endReversed = m_reversed;
        }
    }
}

void SetupEndPopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_groupID -= 1;
    else
        m_groupID += 1;

    updateTargetID();
    updateEditorLabel();
    updateTextInputLabel();
}

void SetupEndPopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    m_groupID = atoi(node->getString().c_str());
    updateTargetID();
    updateEditorLabel();
}

void SetupEndPopup::textInputClosed(CCTextInputNode* node)
{
    textChanged(node);
}

void SetupEndPopup::updateTargetID()
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

void SetupEndPopup::updateTextInputLabel()
{
    m_disableTextDelegate = true;

    m_groupIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_groupID)->getCString());

    m_disableTextDelegate = false;
}

SetupEndPopup::~SetupEndPopup()
{
}