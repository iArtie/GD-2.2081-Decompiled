#include "Popups/Setup/SetupRandTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"
#include "Managers/GameManager.h"
#include "Layers/LevelEditorLayer.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCTextInputNode.h"
#include "UI/InfoAlertButton.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"

SetupRandTriggerPopup* SetupRandTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupRandTriggerPopup* instance = new SetupRandTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupRandTriggerPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_groupID1 = -1;
    m_groupID2 = -1;
    m_chancePercent = -1.0f;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (m_groupID1 == -1)
            m_groupID1 = obj->m_targetGroupID;
        else if (m_groupID1 != obj->m_targetGroupID)
            m_groupID1 = 0;

        if (m_groupID2 == -1)
            m_groupID2 = obj->m_targetGroupID2;
        else if (m_groupID2 != obj->m_targetGroupID2)
            m_groupID2 = 0;

        if (m_chancePercent == -1.0f)
            m_chancePercent = obj->m_duration;
        else if (m_chancePercent != obj->m_duration)
            m_chancePercent = 0.0f;
    }
}

bool SetupRandTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 320.0f, 280.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 18.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupRandTriggerPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Triggers one of two groups <cg>randomly</c>.\n"
        "<cy>Chance</c> is how likely it is that the first group is triggered.\n"
        "For example, setting this to 80% means that there is 80% chance that <cl>Group ID 1</c> is triggered and 20% chanc"
        "e that <cp>Group ID 2</c> is triggered.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    cocos2d::CCPoint groupBase(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f);

    for (int i = 0; i < 2; i++)
    {
        cocos2d::CCPoint pos = groupBase;
        pos.x += (i == 0) ? -75.0f : 75.0f;

        int tag = (i == 0) ? 3 : 5;
        SEL_MenuHandler handler = (i == 0)
            ? menu_selector(SetupRandTriggerPopup::onTargetIDArrow)
            : menu_selector(SetupRandTriggerPopup::onTargetID2Arrow);

        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(
            cocos2d::CCString::createWithFormat("Group ID %i", i + 1)->getCString(), "bigFont.fnt");
        m_mainLayer->addChild(label);
        label->setScale(0.5f);
        label->setPosition(pos + cocos2d::CCPoint(0.0f, 34.0f));

        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
        bg->setColor(cocos2d::ccColor3B(0, 0, 0));
        bg->setOpacity(100);
        bg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(pos);

        CCTextInputNode* input = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
        m_mainLayer->addChild(input, 20);
        input->setPosition(pos);
        input->setMaxLabelScale(0.6f);
        input->setMaxLabelWidth(200.0f);
        input->m_textField->setVisible(false);
        input->m_maxLabelLength = 6;
        input->setAllowedChars("0123456789");
        input->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
        input->setLabelPlaceholderScale(0.5f);
        input->m_delegate = this;
        input->m_numberInput = true;
        input->setTag(tag);

        cocos2d::CCPoint localPos = m_buttonMenu->convertToNodeSpace(pos);

        cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        leftArrow->setScale(1.1f);
        CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(leftArrow, nullptr, handler);
        m_buttonMenu->addChild(leftItem);
        leftItem->setTag(1);
        leftItem->setPosition(localPos + cocos2d::CCPoint(-50.0f, 0.0f));
        leftItem->setSizeMult(2.0f);

        cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        rightArrow->setScale(1.1f);
        CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(rightArrow, nullptr, handler);
        m_buttonMenu->addChild(rightItem);
        rightItem->setTag(2);
        rightItem->setPosition(localPos + cocos2d::CCPoint(50.0f, 0.0f));
        rightItem->setSizeMult(2.0f);

        if (i == 0)
            m_groupID1Input = input;
        else
            m_groupID2Input = input;
    }

    updateTextInputLabel();
    updateTextInputLabel2();

    cocos2d::CCPoint chanceBase(winSize.width * 0.5f, winSize.height * 0.5f - 10.0f);

    cocos2d::CCLabelBMFont* chanceLabel = cocos2d::CCLabelBMFont::create("Chance", "bigFont.fnt");
    chanceLabel->setScale(0.7f);
    chanceLabel->setPosition(chanceBase);
    chanceLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(chanceLabel);

    cocos2d::extension::CCScale9Sprite* chanceBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    chanceBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    chanceBg->setOpacity(100);
    chanceBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(chanceBg, -1);
    chanceBg->setPosition(chanceBase + cocos2d::CCPoint(45.0f, 0.0f));

    m_chanceInput = CCTextInputNode::create(
        50.0f, 40.0f, m_chancePercent == -99999.0f ? "Mixed" : "Num", "Thonburi");
    m_mainLayer->addChild(m_chanceInput, 20);
    m_chanceInput->setPosition(chanceBg->getPosition());
    m_chanceInput->setMaxLabelScale(0.6f);
    m_chanceInput->setMaxLabelWidth(200.0f);
    m_chanceInput->m_textField->setVisible(false);
    m_chanceInput->m_maxLabelLength = 7;
    m_chanceInput->setAllowedChars("0123456789.");
    m_chanceInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_chanceInput->setLabelPlaceholderScale(0.5f);
    m_chanceInput->m_delegate = this;
    m_chanceInput->m_numberInput = true;
    m_chanceInput->setTag(4);

    m_chanceSlider = Slider::create(this, menu_selector(SetupRandTriggerPopup::sliderChanged), 0, 0.8f);
    m_chanceSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_chanceSlider);
    m_chanceSlider->setPosition(chanceBase + cocos2d::CCPoint(0.0f, -32.0f));

    float chanceValue = m_chancePercent / 100.0f;
    if (chanceValue >= 100.0f)
        chanceValue = 100.0f;
    else if (chanceValue <= 0.0f)
        chanceValue = 0.0f;
    m_chanceSlider->setValue(chanceValue);

    updateChanceLabel(false);

    createMultiTriggerItemsDefault();

    m_disableTextDelegate = false;

    return true;
}

void SetupRandTriggerPopup::onClose(cocos2d::CCObject* sender)
{
    m_groupID1Input->m_delegate = nullptr;
    m_groupID1Input->onClickTrackNode(false);
    m_groupID2Input->m_delegate = nullptr;
    m_groupID2Input->onClickTrackNode(false);
    m_chanceInput->m_delegate = nullptr;
    m_chanceInput->onClickTrackNode(false);
    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    SetupTriggerPopup::onClose(sender);
}

void SetupRandTriggerPopup::onTargetID2Arrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_groupID2 -= 1;
    else
        m_groupID2 += 1;

    updateTargetID2();
    updateEditorLabel();
    updateTextInputLabel2();
}

void SetupRandTriggerPopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_groupID1 -= 1;
    else
        m_groupID1 += 1;

    updateTargetID();
    updateEditorLabel();
    updateTextInputLabel();
}

void SetupRandTriggerPopup::sliderChanged(cocos2d::CCObject* sender)
{
    m_chancePercent = roundf(((SliderThumb*)sender)->getValue() * 100.0f);
    updateChance();
    updateChanceLabel(false);
}

void SetupRandTriggerPopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    if (node->getTag() == 3)
    {
        m_groupID1 = atoi(node->getString().c_str());
        updateTargetID();
        updateEditorLabel();
    }
    else if (node->getTag() == 5)
    {
        m_groupID2 = atoi(node->getString().c_str());
        updateTargetID2();
        updateEditorLabel();
    }
    else if (node->getTag() == 4)
    {
        m_chancePercent = cocos2d::CCString::createWithFormat(
            "%.04f", atof(node->getString().c_str()))->floatValue();
        updateChance();
    }
}

void SetupRandTriggerPopup::updateChance()
{
    if (m_gameObject)
    {
        m_gameObject->m_duration = m_chancePercent;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_duration = m_chancePercent;
        }
    }
}

void SetupRandTriggerPopup::updateChanceLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_chancePercent == -99999.0f)
        text = "Mixed";
    else if (!decimals && m_chancePercent - (float)(int)m_chancePercent == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_chancePercent)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.04f", m_chancePercent)->getCString();

    m_chanceInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupRandTriggerPopup::updateTargetID()
{
    if (m_groupID1 < 0)
        m_groupID1 = 0;
    if (m_groupID1 > 9999)
        m_groupID1 = 9999;

    if (m_gameObject)
    {
        m_gameObject->setTargetID(m_groupID1);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->setTargetID(m_groupID1);
        }
    }
}

void SetupRandTriggerPopup::updateTargetID2()
{
    if (m_groupID2 < 0)
        m_groupID2 = 0;
    if (m_groupID2 > 9999)
        m_groupID2 = 9999;

    if (m_gameObject)
    {
        m_gameObject->setTargetID2(m_groupID2);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->setTargetID2(m_groupID2);
        }
    }
}

void SetupRandTriggerPopup::updateTextInputLabel()
{
    m_disableTextDelegate = true;

    m_groupID1Input->setString(
        cocos2d::CCString::createWithFormat("%i", m_groupID1)->getCString());

    m_disableTextDelegate = false;
}

void SetupRandTriggerPopup::updateTextInputLabel2()
{
    m_disableTextDelegate = true;

    m_groupID2Input->setString(
        cocos2d::CCString::createWithFormat("%i", m_groupID2)->getCString());

    m_disableTextDelegate = false;
}

SetupRandTriggerPopup::~SetupRandTriggerPopup()
{
}