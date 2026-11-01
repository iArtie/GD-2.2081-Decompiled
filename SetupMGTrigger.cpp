#include "SetupMGTrigger.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "ConfigureValuePopup.h"
#include "InfoAlertButton.h"
#include "Slider.h"
#include "SliderThumb.h"

SetupMGTrigger* SetupMGTrigger::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupMGTrigger* instance = new SetupMGTrigger();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupMGTrigger::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    m_mgEasingType = (EasingType)-99999;
    m_offsetY = -99999;
    m_moveTime = -99999.0f;
    m_mgEasingRate = -99999.0f;

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    float moveTime = -99999.0f;
    float easingRate = -99999.0f;
    int offsetY = -99999;
    EasingType easingType = (EasingType)-99999;
    int moveTimeConflict = 0;
    int easingRateConflict = 0;
    int offsetConflict = 0;
    int easingConflict = 0;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!moveTimeConflict)
        {
            if (moveTime == -99999.0f)
                moveTime = obj->m_duration;
            else if (moveTime != obj->m_duration)
            {
                moveTime = -99999.0f;
                moveTimeConflict = 1;
            }
        }

        if (!easingConflict)
        {
            if (easingType == (EasingType)-99999)
                easingType = obj->m_easingType;
            else if (easingType != obj->m_easingType)
            {
                easingType = (EasingType)-99999;
                easingConflict = 1;
            }
        }

        if (!easingRateConflict)
        {
            if (easingRate == -99999.0f)
                easingRate = obj->m_easingRate;
            else if (easingRate != obj->m_easingRate)
            {
                easingRate = -99999.0f;
                easingRateConflict = 1;
            }
        }

        if (!offsetConflict)
        {
            if (offsetY == -99999)
                offsetY = (int)obj->m_moveOffset.y;
            else if (offsetY != (int)obj->m_moveOffset.y)
            {
                offsetY = -99999;
                offsetConflict = 1;
            }
        }
    }

    if (moveTime != -99999.0f)
        m_moveTime = moveTime;
    if (easingType != (EasingType)-99999)
        m_mgEasingType = easingType;
    if (easingRate != -99999.0f)
        m_mgEasingRate = easingRate;
    if (offsetY != -99999)
        m_offsetY = offsetY;
}

bool SetupMGTrigger::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 430.0f, 310.0f, 1))
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 15.0f);
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupTriggerPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help", "Lets you change the Y position of the <cg>middle ground</c>.", 1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(m_width * 0.5f + winSize.width * 0.5f - 18.0f,
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    determineStartValues();

    cocos2d::CCPoint moveTimeBase(winSize.width * 0.5f, winSize.height * 0.5f - 60.0f);

    cocos2d::CCLabelBMFont* moveTimeLabel = cocos2d::CCLabelBMFont::create("Move Time", "bigFont.fnt");
    moveTimeLabel->setScale(0.7f);
    moveTimeLabel->setPosition(moveTimeBase);
    moveTimeLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(moveTimeLabel);

    cocos2d::extension::CCScale9Sprite* moveTimeBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    moveTimeBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    moveTimeBg->setOpacity(100);
    moveTimeBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(moveTimeBg, -1);
    moveTimeBg->setPosition(moveTimeBase + cocos2d::CCPoint(45.0f, 0.0f));

    m_moveTimeInput = CCTextInputNode::create(
        50.0f, 40.0f, m_moveTime == -99999.0f ? "Mixed" : "Num", "Thonburi");
    m_mainLayer->addChild(m_moveTimeInput, 20);
    m_moveTimeInput->setPosition(moveTimeBg->getPosition());
    m_moveTimeInput->setMaxLabelScale(0.6f);
    m_moveTimeInput->setMaxLabelWidth(200.0f);
    m_moveTimeInput->m_textField->setVisible(false);
    m_moveTimeInput->m_maxLabelLength = 4;
    m_moveTimeInput->setAllowedChars("0123456789.");
    m_moveTimeInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_moveTimeInput->setLabelPlaceholderScale(0.5f);
    m_moveTimeInput->m_delegate = this;
    m_moveTimeInput->m_numberInput = true;
    m_moveTimeInput->setTag(3);

    m_moveTimeSlider = Slider::create(this, menu_selector(SetupMGTrigger::sliderChanged), 0, 0.8f);
    m_moveTimeSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_moveTimeSlider);
    m_moveTimeSlider->setPosition(moveTimeBase + cocos2d::CCPoint(0.0f, -32.0f));

    float moveTimeValue = m_moveTime / 10.0f;
    if (moveTimeValue >= 1.0f)
        moveTimeValue = 1.0f;
    else if (moveTimeValue <= 0.0f)
        moveTimeValue = 0.0f;
    m_moveTimeSlider->setValue(moveTimeValue);

    updateDurLabel(false);

    cocos2d::CCLabelBMFont* easingLabel = cocos2d::CCLabelBMFont::create("Easing", "bigFont.fnt");
    easingLabel->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f);
    m_mainLayer->addChild(easingLabel);
    easingLabel->setScale(0.8f);

    m_mgEasingLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_mgEasingLabel);
    m_mgEasingLabel->setPosition(easingLabel->getPosition() + cocos2d::CCPoint(0.0f, -30.0f));
    m_mgEasingLabel->setScale(0.7f);

    cocos2d::CCSprite* easingRateSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    easingRateSprite->setColor(cocos2d::ccColor3B(255, 255, 255));
    easingRateSprite->setOpacity(100);

    m_mgEasingRateLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    easingRateSprite->addChild(m_mgEasingRateLabel);
    m_mgEasingRateLabel->setScaleX(0.375f);
    m_mgEasingRateLabel->setScaleY(0.75f);
    m_mgEasingRateLabel->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    easingRateSprite->setScaleX(0.8f);
    easingRateSprite->setScaleY(0.4f);

    m_mgEasingRateButton = CCMenuItemSpriteExtra::create(
        easingRateSprite, nullptr, menu_selector(SetupMGTrigger::onEasingRate));
    m_buttonMenu->addChild(m_mgEasingRateButton);
    m_mgEasingRateButton->setPosition(m_buttonMenu->convertToNodeSpace(
        easingLabel->getPosition() + cocos2d::CCPoint(55.0f, -2.0f)));
    m_mgEasingRateButton->setSizeMult(1.2f);

    updateEasingRateLabel();
    toggleEasingRateVisibility();

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftArrow->setFlipX(true);
    leftArrow->setScale(0.7f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupMGTrigger::onEasing));
    m_buttonMenu->addChild(leftItem);
    leftItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_mgEasingLabel->getPosition() + cocos2d::CCPoint(100.0f, 0.0f)));
    leftItem->setSizeMult(1.4f);
    leftItem->setTag(1);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrow->setScale(0.7f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupMGTrigger::onEasing));
    m_buttonMenu->addChild(rightItem);
    rightItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_mgEasingLabel->getPosition() + cocos2d::CCPoint(-100.0f, 0.0f)));
    rightItem->setSizeMult(1.4f);
    rightItem->setTag(0);

    cocos2d::CCPoint offsetBase(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f);

    cocos2d::CCLabelBMFont* offsetLabel = cocos2d::CCLabelBMFont::create("Offset Y", "bigFont.fnt");
    offsetLabel->setScale(0.7f);
    offsetLabel->setPosition(offsetBase);
    offsetLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(offsetLabel);

    cocos2d::extension::CCScale9Sprite* offsetBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    offsetBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    offsetBg->setOpacity(100);
    offsetBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(offsetBg, -1);
    offsetBg->setPosition(offsetBase + cocos2d::CCPoint(45.0f, 0.0f));

    m_offsetYInput = CCTextInputNode::create(
        60.0f, 40.0f, m_offsetY == -99999 ? "Mixed" : "Num", "Thonburi");
    m_mainLayer->addChild(m_offsetYInput, 20);
    m_offsetYInput->setPosition(offsetBg->getPosition());
    m_offsetYInput->setMaxLabelScale(0.6f);
    m_offsetYInput->setMaxLabelWidth(200.0f);
    m_offsetYInput->m_textField->setVisible(false);
    m_offsetYInput->m_maxLabelLength = 5;
    m_offsetYInput->setAllowedChars("0123456789-");
    m_offsetYInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_offsetYInput->setLabelPlaceholderScale(0.5f);
    m_offsetYInput->m_delegate = this;
    m_offsetYInput->m_numberInput = true;
    m_offsetYInput->setTag(1);

    m_offsetYSlider = Slider::create(this, menu_selector(SetupMGTrigger::sliderYChanged), 0, 0.8f);
    m_offsetYSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_offsetYSlider);
    m_offsetYSlider->setPosition(offsetBase + cocos2d::CCPoint(0.0f, -32.0f));

    float offsetValue = (m_offsetY == -99999) ? 0.5f : sliderValueFromPos(m_offsetY);
    m_offsetYSlider->setValue(offsetValue);

    createMultiTriggerItemsDefault();

    updateValueYLabel();
    updateEasingLabel();

    m_disableTextDelegate = false;

    return true;
}

void SetupMGTrigger::onClose(cocos2d::CCObject* sender)
{
    m_moveTimeInput->m_delegate = nullptr;
    m_moveTimeInput->onClickTrackNode(false);
    m_offsetYInput->m_delegate = nullptr;
    m_offsetYInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupMGTrigger::onEasing(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_mgEasingType = (EasingType)((int)m_mgEasingType + 1);
    else
        m_mgEasingType = (EasingType)((int)m_mgEasingType - 1);

    if ((int)m_mgEasingType < 0)
        m_mgEasingType = (EasingType)0;
    if ((int)m_mgEasingType > (int)EasingType::BackOut)
        m_mgEasingType = EasingType::BackOut;

    updateMoveCommandEasing();
    updateEasingLabel();
    toggleEasingRateVisibility();
}

void SetupMGTrigger::onEasingRate(cocos2d::CCObject* sender)
{
    ConfigureValuePopup* popup = ConfigureValuePopup::create(
        this, m_mgEasingRate, 0.1f, 4.0f, "Easing Rate", "", 0);
    popup->show();
}

int SetupMGTrigger::posFromSliderValue(float value)
{
    return (int)(floorf((value - 0.5f) * 200.0f) * 3.0f);
}

void SetupMGTrigger::sliderChanged(cocos2d::CCObject* sender)
{
    m_moveTime = ((SliderThumb*)sender)->getValue() * 10.0f;
    updateDuration();
    updateDurLabel(true);
}

float SetupMGTrigger::sliderValueFromPos(int pos)
{
    float value = floorf((float)pos / 3.0f) / 200.0f + 0.5f;
    if (value >= 1.0f)
        return 1.0f;
    if (value <= 0.0f)
        return 0.0f;
    return value;
}

void SetupMGTrigger::sliderYChanged(cocos2d::CCObject* sender)
{
    m_offsetY = posFromSliderValue(((SliderThumb*)sender)->getValue());
    updateValueYLabel();
    updateMoveCommandPosY();
}

void SetupMGTrigger::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    int tag = node->getTag();

    if (tag == 1)
    {
        m_offsetY = (int)((float)atoi(node->getString().c_str()) * 3.0f);
        updateMoveCommandPosY();

        float value = sliderValueFromPos(m_offsetY);
        m_offsetYSlider->setValue(value < 1.0f ? value : 1.0f);
    }
    else if (tag == 3)
    {
        m_moveTime = cocos2d::CCString::createWithFormat(
            "%.02f", atof(node->getString().c_str()))->floatValue();
        updateDuration();

        float value = m_moveTime / 10.0f;
        if (value >= 1.0f)
            value = 1.0f;
        else if (value <= 0.0f)
            value = 0.0f;
        m_moveTimeSlider->setValue(value);
    }
}

void SetupMGTrigger::toggleEasingRateVisibility()
{
    bool visible = GameToolbox::isRateEasing(m_mgEasingType);

    if (!visible && m_gameObjects && m_gameObjects->count() > 0)
    {
        visible = true;
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            if (!GameToolbox::isRateEasing(obj->m_easingType))
            {
                visible = false;
                break;
            }
        }
    }

    m_mgEasingRateButton->setVisible(visible);
}

void SetupMGTrigger::updateDurLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_moveTime == -99999.0f)
        text = "Mixed";
    else if (!decimals && m_moveTime - (float)(int)m_moveTime == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_moveTime)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_moveTime)->getCString();

    m_moveTimeInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupMGTrigger::updateDuration()
{
    if (m_gameObject)
    {
        m_gameObject->m_duration = m_moveTime;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_duration = m_moveTime;
        }
    }
}

void SetupMGTrigger::updateEasingLabel()
{
    std::string text = "Mixed";
    if (m_mgEasingType != (EasingType)-99999)
        text = GameToolbox::easeToText(m_mgEasingType);

    m_mgEasingLabel->setString(text.c_str());
    m_mgEasingLabel->limitLabelWidth(150.0f, 0.7f, 0.0f);
}

void SetupMGTrigger::updateEasingRateLabel()
{
    std::string text = cocos2d::CCString::createWithFormat("%.02f", m_mgEasingRate)->getCString();
    if (m_mgEasingRate == -99999.0f)
        text = "";

    m_mgEasingRateLabel->setString(text.c_str());
}

void SetupMGTrigger::updateMoveCommandEasing()
{
    if (m_gameObject)
    {
        m_gameObject->m_easingType = m_mgEasingType;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_easingType = m_mgEasingType;
        }
    }
}

void SetupMGTrigger::updateMoveCommandEasingRate()
{
    if (m_gameObject)
    {
        m_gameObject->m_easingRate = m_mgEasingRate;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_easingRate = m_mgEasingRate;
        }
    }
}

void SetupMGTrigger::updateMoveCommandPosY()
{
    if (m_gameObject)
    {
        m_gameObject->m_moveOffset = cocos2d::CCPoint(m_gameObject->m_moveOffset.x, (float)m_offsetY);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_moveOffset = cocos2d::CCPoint(obj->m_moveOffset.x, (float)m_offsetY);
        }
    }
}

void SetupMGTrigger::updateValueYLabel()
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_offsetY == -99999)
        text = "";
    else
        text = cocos2d::CCString::createWithFormat("%i", (int)floorf((float)m_offsetY / 3.0f))->getCString();

    m_offsetYInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupMGTrigger::valuePopupClosed(ConfigureValuePopup* popup, float value)
{
    float clamped = cocos2d::CCString::createWithFormat("%.02f", value)->floatValue();
    if (clamped >= 20.0f)
        clamped = 20.0f;
    else if (clamped <= 0.1f)
        clamped = 0.1f;

    m_mgEasingRate = clamped;
    updateMoveCommandEasingRate();
    updateEasingRateLabel();
}

SetupMGTrigger::~SetupMGTrigger()
{
}