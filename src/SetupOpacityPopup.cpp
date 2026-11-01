#include "SetupOpacityPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "InfoAlertButton.h"
#include "Slider.h"
#include "SliderThumb.h"

SetupOpacityPopup* SetupOpacityPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupOpacityPopup* instance = new SetupOpacityPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupOpacityPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_groupID = -99999;
    float fadeTime = -99999.0f;
    float opacity = -99999.0f;
    m_opacity = 1.0f;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (fadeTime == -99999.0f)
            fadeTime = obj->m_duration;
        else if (fadeTime != obj->m_duration)
            fadeTime = -2.0f;

        if (opacity == -99999.0f)
            opacity = obj->m_opacity;
        else if (opacity != obj->m_opacity)
            opacity = -2.0f;

        if (m_groupID == -99999)
            m_groupID = obj->m_targetGroupID;
        else if (m_groupID != obj->m_targetGroupID)
            m_groupID = 0;
    }

    if (fadeTime >= 0.0f)
        m_fadeTime = fadeTime;
    if (opacity >= 0.0f)
        m_opacity = opacity;
}

bool SetupOpacityPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 360.0f, 290.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 18.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupOpacityPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Changes the <cg>opacity</c> of a group. If any objects in the group have a color channel with opacity, the values are multiplied.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    cocos2d::CCPoint groupIDPos(winSize.width * 0.5f, winSize.height * 0.5f + 65.0f);

    cocos2d::CCLabelBMFont* groupIDLabel = cocos2d::CCLabelBMFont::create("Group ID:", "bigFont.fnt");
    m_mainLayer->addChild(groupIDLabel);
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
    m_groupIDInput->setTag(3);

    cocos2d::CCPoint inputPos = m_buttonMenu->convertToNodeSpace(groupIDPos);

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupOpacityPopup::onTargetIDArrow));
    m_buttonMenu->addChild(leftItem);
    leftItem->setTag(1);
    leftItem->setPosition(inputPos + cocos2d::CCPoint(-50.0f, 0.0f));
    leftItem->setSizeMult(2.0f);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupOpacityPopup::onTargetIDArrow));
    m_buttonMenu->addChild(rightItem);
    rightItem->setTag(2);
    rightItem->setPosition(inputPos + cocos2d::CCPoint(50.0f, 0.0f));
    rightItem->setSizeMult(2.0f);

    updateTextInputLabel();

    cocos2d::CCPoint fadeTimePos(winSize.width * 0.5f, winSize.height * 0.5f + 20.0f);

    cocos2d::CCLabelBMFont* fadeTimeLabel = cocos2d::CCLabelBMFont::create("Fade Time", "bigFont.fnt");
    fadeTimeLabel->setScale(0.7f);
    fadeTimeLabel->setPosition(fadeTimePos);
    fadeTimeLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(fadeTimeLabel);

    cocos2d::extension::CCScale9Sprite* fadeTimeBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    fadeTimeBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    fadeTimeBg->setOpacity(100);
    fadeTimeBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(fadeTimeBg, -1);
    fadeTimeBg->setPosition(fadeTimePos + cocos2d::CCPoint(45.0f, 0.0f));

    m_fadeTimeInput = CCTextInputNode::create(
        50.0f, 40.0f, m_fadeTime == -99999.0f ? "Mixed" : "Num", "Thonburi");
    m_mainLayer->addChild(m_fadeTimeInput, 20);
    m_fadeTimeInput->setPosition(fadeTimeBg->getPosition());
    m_fadeTimeInput->setMaxLabelScale(0.6f);
    m_fadeTimeInput->setMaxLabelWidth(200.0f);
    m_fadeTimeInput->m_textField->setVisible(false);
    m_fadeTimeInput->m_maxLabelLength = 4;
    m_fadeTimeInput->setAllowedChars("0123456789.");
    m_fadeTimeInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_fadeTimeInput->setLabelPlaceholderScale(0.5f);
    m_fadeTimeInput->m_delegate = this;
    m_fadeTimeInput->m_numberInput = true;
    m_fadeTimeInput->setTag(4);

    m_fadeTimeSlider = Slider::create(this, menu_selector(SetupOpacityPopup::sliderChanged), 0, 0.8f);
    m_mainLayer->addChild(m_fadeTimeSlider);
    m_fadeTimeSlider->setPosition(fadeTimePos + cocos2d::CCPoint(0.0f, -32.0f));

    float fadeTimeValue = m_fadeTime / 10.0f;
    if (fadeTimeValue >= 1.0f)
        fadeTimeValue = 1.0f;
    else if (fadeTimeValue <= 0.0f)
        fadeTimeValue = 0.0f;
    m_fadeTimeSlider->setValue(fadeTimeValue);
    m_fadeTimeSlider->getThumb()->setTag(1);

    updateDurLabel(false);

    m_opacityLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_opacityLabel);
    m_opacityLabel->setPosition(winSize.width * 0.5f, winSize.height * 0.5f - 40.0f);
    m_opacityLabel->setScale(0.8f);

    m_opacitySlider = Slider::create(this, menu_selector(SetupOpacityPopup::sliderChanged));
    m_mainLayer->addChild(m_opacitySlider);
    m_opacitySlider->setPosition(m_opacityLabel->getPosition() + cocos2d::CCPoint(0.0f, -23.0f));
    m_opacitySlider->getThumb()->setTag(2);

    updateOpacityLabel();
    m_opacitySlider->setValue(m_opacity);

    createMultiTriggerItemsDefault();

    m_disableTextDelegate = false;

    return true;
}

void SetupOpacityPopup::onClose(cocos2d::CCObject* sender)
{
    m_groupIDInput->m_delegate = nullptr;
    m_groupIDInput->onClickTrackNode(false);
    m_fadeTimeInput->m_delegate = nullptr;
    m_fadeTimeInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupOpacityPopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_groupID -= 1;
    else
        m_groupID += 1;

    updateTargetID();
    updateEditorLabel();
    updateTextInputLabel();
}

void SetupOpacityPopup::sliderChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();

    if (sender->getTag() == 2)
    {
        float opacity = cocos2d::CCString::createWithFormat("%.02f", value)->floatValue();
        if (opacity > 0.02f)
        {
            if (opacity >= 0.98f)
                opacity = 1.0f;
            m_opacity = opacity;
        }
        else
        {
            m_opacity = 0.0f;
        }
        updateOpacity();
        updateOpacityLabel();
    }
    else if (sender->getTag() == 1)
    {
        m_fadeTime = cocos2d::CCString::createWithFormat("%.02f", value * 10.0f)->floatValue();
        updateDuration();
        updateDurLabel(true);
    }
}

void SetupOpacityPopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    if (node->getTag() == 3)
    {
        m_groupID = atoi(node->getString().c_str());
        updateTargetID();
        updateEditorLabel();
    }
    else if (node->getTag() == 4)
    {
        m_fadeTime = cocos2d::CCString::createWithFormat(
            "%.02f", atof(node->getString().c_str()))->floatValue();
        updateDuration();

        float value = m_fadeTime / 10.0f;
        if (value >= 1.0f)
            value = 1.0f;
        else if (value <= 0.0f)
            value = 0.0f;
        m_fadeTimeSlider->setValue(value);
    }
}

void SetupOpacityPopup::textInputClosed(CCTextInputNode* node)
{
    textChanged(node);
}

void SetupOpacityPopup::updateDurLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_fadeTime == -99999.0f)
        text = "Mixed";
    else if (!decimals && m_fadeTime - (float)(int)m_fadeTime == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_fadeTime)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_fadeTime)->getCString();

    m_fadeTimeInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupOpacityPopup::updateDuration()
{
    if (m_gameObject)
    {
        m_gameObject->m_duration = m_fadeTime;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_duration = m_fadeTime;
        }
    }
}

void SetupOpacityPopup::updateOpacity()
{
    if (m_gameObject)
    {
        m_gameObject->m_opacity = m_opacity;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_opacity = m_opacity;
        }
    }
}

void SetupOpacityPopup::updateOpacityLabel()
{
    m_opacityLabel->setString(
        cocos2d::CCString::createWithFormat("Opacity: %.02f", m_opacity)->getCString());
}

void SetupOpacityPopup::updateTargetID()
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

void SetupOpacityPopup::updateTextInputLabel()
{
    m_disableTextDelegate = true;

    m_groupIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_groupID)->getCString());

    m_disableTextDelegate = false;
}

SetupOpacityPopup::~SetupOpacityPopup()
{
}