#include "SetupCameraRotatePopup.h"
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

SetupCameraRotatePopup* SetupCameraRotatePopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupCameraRotatePopup* instance = new SetupCameraRotatePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupCameraRotatePopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    m_cameraEasingType = (EasingType)-99999;
    m_moveTime = -99999.0f;
    m_degrees = -99999.0f;
    m_cameraEasingRate = -99999.0f;

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    float moveTime = -99999.0f;
    float degrees = -99999.0f;
    float easingRate = -99999.0f;
    EasingType easingType = (EasingType)-99999;

    int moveTimeConflict = 0;
    int degreesConflict = 0;
    int easingRateConflict = 0;
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
                easingConflict = 1;
                easingType = (EasingType)-99999;
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

        if (!degreesConflict)
        {
            if (degrees == -99999.0f)
                degrees = obj->m_rotationDegrees;
            else if (degrees != obj->m_rotationDegrees)
            {
                degrees = -99999.0f;
                degreesConflict = 1;
            }
        }
    }

    if (moveTime != -99999.0f)
        m_moveTime = moveTime;
    if (easingType != (EasingType)-99999)
        m_cameraEasingType = easingType;
    if (easingRate != -99999.0f)
        m_cameraEasingRate = easingRate;
    m_degrees = degrees;
}

bool SetupCameraRotatePopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
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
        okButton, nullptr, menu_selector(SetupCameraRotatePopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "<cg>Rotates</c> the camera to the target value.\n"
        "<cy>Move Time</c> is how fast the rotate happens.\n"
        "<cl>Easing</c> changes the method used.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(m_width * 0.5f + winSize.width * 0.5f - 18.0f,
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    determineStartValues();

    cocos2d::CCPoint moveTimeLabelPos(winSize.width * 0.5f - 90.0f, winSize.height * 0.5f - 50.0f);

    cocos2d::CCLabelBMFont* moveTimeLabel = cocos2d::CCLabelBMFont::create("Move Time", "bigFont.fnt");
    moveTimeLabel->setScale(0.7f);
    moveTimeLabel->setPosition(moveTimeLabelPos);
    moveTimeLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(moveTimeLabel);

    cocos2d::extension::CCScale9Sprite* moveTimeBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    moveTimeBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    moveTimeBg->setOpacity(100);
    moveTimeBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(moveTimeBg, -1);
    moveTimeBg->setPosition(moveTimeLabelPos + cocos2d::CCPoint(45.0f, 0.0f));

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
    m_moveTimeInput->setTag(2);

    m_moveTimeSlider = Slider::create(this, menu_selector(SetupCameraRotatePopup::sliderChanged), 0, 0.8f);
    m_moveTimeSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_moveTimeSlider);
    m_moveTimeSlider->setPosition(moveTimeLabelPos + cocos2d::CCPoint(0.0f, -32.0f));

    float moveTimeValue = m_moveTime / 10.0f;
    if (moveTimeValue >= 1.0f)
        moveTimeValue = 1.0f;
    else if (moveTimeValue <= 0.0f)
        moveTimeValue = 0.0f;
    m_moveTimeSlider->setValue(moveTimeValue);

    updateDurLabel(false);

    cocos2d::CCLabelBMFont* easingLabel = cocos2d::CCLabelBMFont::create("Easing", "bigFont.fnt");
    easingLabel->setPosition(winSize.width * 0.5f - 90.0f, winSize.height * 0.5f + 20.0f);
    m_mainLayer->addChild(easingLabel);
    easingLabel->setScale(0.8f);

    m_cameraEasingLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_cameraEasingLabel);
    m_cameraEasingLabel->setPosition(easingLabel->getPosition() + cocos2d::CCPoint(0.0f, -30.0f));
    m_cameraEasingLabel->setScale(0.7f);

    cocos2d::CCSprite* easingRateSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    easingRateSprite->setColor(cocos2d::ccColor3B(255, 255, 255));
    easingRateSprite->setOpacity(100);

    m_cameraEasingRateLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    easingRateSprite->addChild(m_cameraEasingRateLabel);
    m_cameraEasingRateLabel->setScaleX(0.375f);
    m_cameraEasingRateLabel->setScaleY(0.75f);
    m_cameraEasingRateLabel->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    easingRateSprite->setScaleX(0.8f);
    easingRateSprite->setScaleY(0.4f);

    m_cameraEasingRateButton = CCMenuItemSpriteExtra::create(
        easingRateSprite, nullptr, menu_selector(SetupCameraRotatePopup::onEasingRate));
    m_buttonMenu->addChild(m_cameraEasingRateButton);
    m_cameraEasingRateButton->setPosition(m_buttonMenu->convertToNodeSpace(
        easingLabel->getPosition() + cocos2d::CCPoint(55.0f, -2.0f)));
    m_cameraEasingRateButton->setSizeMult(1.2f);

    updateEasingRateLabel();
    toggleEasingRateVisibility();

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftArrow->setFlipX(true);
    leftArrow->setScale(0.7f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupCameraRotatePopup::onEasing));
    m_buttonMenu->addChild(leftItem);
    leftItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_cameraEasingLabel->getPosition() + cocos2d::CCPoint(100.0f, 0.0f)));
    leftItem->setSizeMult(2.0f);
    leftItem->setTag(1);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrow->setScale(0.7f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupCameraRotatePopup::onEasing));
    m_buttonMenu->addChild(rightItem);
    rightItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_cameraEasingLabel->getPosition() + cocos2d::CCPoint(-100.0f, 0.0f)));
    rightItem->setSizeMult(2.0f);
    rightItem->setTag(0);

    cocos2d::CCPoint degreesLabelPos(moveTimeLabelPos.x + 180.0f, moveTimeLabelPos.y);

    cocos2d::CCLabelBMFont* degreesLabel = cocos2d::CCLabelBMFont::create("Degrees", "bigFont.fnt");
    degreesLabel->setScale(0.7f);
    degreesLabel->setPosition(degreesLabelPos);
    degreesLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(degreesLabel);

    cocos2d::extension::CCScale9Sprite* degreesBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    degreesBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    degreesBg->setOpacity(100);
    degreesBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(degreesBg, -1);
    degreesBg->setPosition(degreesLabelPos + cocos2d::CCPoint(45.0f, 0.0f));

    m_degrees = (float)(int)m_degrees;

    m_degreesInput = CCTextInputNode::create(
        60.0f, 40.0f, m_degrees == -99999.0f ? "Mixed" : "Num", "Thonburi");
    m_mainLayer->addChild(m_degreesInput, 20);
    m_degreesInput->setPosition(degreesBg->getPosition());
    m_degreesInput->setMaxLabelScale(0.6f);
    m_degreesInput->setMaxLabelWidth(200.0f);
    m_degreesInput->m_textField->setVisible(false);
    m_degreesInput->m_maxLabelLength = 6;
    m_degreesInput->setAllowedChars("0123456789-.");
    m_degreesInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_degreesInput->setLabelPlaceholderScale(0.5f);
    m_degreesInput->m_delegate = this;
    m_degreesInput->m_numberInput = true;
    m_degreesInput->setTag(0);

    m_degreesSlider = Slider::create(this, menu_selector(SetupCameraRotatePopup::sliderDegreesChanged), 0, 0.8f);
    m_degreesSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_degreesSlider);
    m_degreesSlider->setPosition(degreesLabelPos + cocos2d::CCPoint(0.0f, -32.0f));

    float degreesValue = (m_degrees == -99999.0f) ? 0.5f : (m_degrees / 720.0f + 0.5f);
    if (degreesValue >= 1.0f)
        degreesValue = 1.0f;
    else if (degreesValue <= 0.0f)
        degreesValue = 0.0f;
    m_degreesSlider->setValue(degreesValue);

    createMultiTriggerItemsDefaultHorizontal();

    updateDegreesLabel();
    updateEasingLabel();

    m_disableTextDelegate = false;

    return true;
}

void SetupCameraRotatePopup::onClose(cocos2d::CCObject* sender)
{
    m_moveTimeInput->m_delegate = nullptr;
    m_moveTimeInput->onClickTrackNode(false);
    m_degreesInput->m_delegate = nullptr;
    m_degreesInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupCameraRotatePopup::onEasing(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag == 1)
        m_cameraEasingType = (EasingType)((int)m_cameraEasingType + 1);
    else
        m_cameraEasingType = (EasingType)((int)m_cameraEasingType - 1);

    if ((int)m_cameraEasingType < 0)
        m_cameraEasingType = (EasingType)0;
    if ((int)m_cameraEasingType > (int)EasingType::BackOut)
        m_cameraEasingType = EasingType::BackOut;

    updateMoveCommandEasing();
    updateEasingLabel();
    toggleEasingRateVisibility();
}

void SetupCameraRotatePopup::onEasingRate(cocos2d::CCObject* sender)
{
    ConfigureValuePopup* popup = ConfigureValuePopup::create(
        this, m_cameraEasingRate, 0.1f, 4.0f, "Easing Rate", "", 0);
    popup->show();
}

void SetupCameraRotatePopup::sliderChanged(cocos2d::CCObject* sender)
{
    m_moveTime = ((SliderThumb*)sender)->getValue() * 10.0f;
    updateDuration();
    updateDurLabel(true);
}

void SetupCameraRotatePopup::sliderDegreesChanged(cocos2d::CCObject* sender)
{
    m_degrees = floorf(((SliderThumb*)sender)->getValue() * 720.0f) - 360.0f;
    updateDegreesLabel();
    updateCommandDegrees();
}

void SetupCameraRotatePopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    int tag = node->getTag();

    if (tag == 2)
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
    else if (tag == 0)
    {
        m_degrees = cocos2d::CCString::createWithFormat(
            "%.02f", atof(node->getString().c_str()))->floatValue();
        updateCommandDegrees();

        float value = m_degrees / 720.0f + 0.5f;
        if (value >= 1.0f)
            value = 1.0f;
        else if (value <= 0.0f)
            value = 0.0f;
        m_degreesSlider->setValue(value);
    }
}

void SetupCameraRotatePopup::toggleEasingRateVisibility()
{
    bool visible = GameToolbox::isRateEasing(m_cameraEasingType);

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

    m_cameraEasingRateButton->setVisible(visible);
}

void SetupCameraRotatePopup::updateCommandDegrees()
{
    if (m_gameObject)
    {
        m_gameObject->m_rotationDegrees = m_degrees;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_rotationDegrees = m_degrees;
        }
    }
}

void SetupCameraRotatePopup::updateDegreesLabel()
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_degrees == -99999.0f)
        text = "";
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_degrees)->getCString();

    m_degreesInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupCameraRotatePopup::updateDurLabel(bool decimals)
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

void SetupCameraRotatePopup::updateDuration()
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

void SetupCameraRotatePopup::updateEasingLabel()
{
    std::string text = "Mixed";
    if (m_cameraEasingType != (EasingType)-99999)
        text = GameToolbox::easeToText(m_cameraEasingType);

    m_cameraEasingLabel->setString(text.c_str());
    m_cameraEasingLabel->limitLabelWidth(150.0f, 0.7f, 0.0f);
}

void SetupCameraRotatePopup::updateEasingRateLabel()
{
    std::string text = cocos2d::CCString::createWithFormat("%.02f", m_cameraEasingRate)->getCString();
    if (m_cameraEasingRate == -99999.0f)
        text = "";

    m_cameraEasingRateLabel->setString(text.c_str());
}

void SetupCameraRotatePopup::updateMoveCommandEasing()
{
    if (m_gameObject)
    {
        m_gameObject->m_easingType = m_cameraEasingType;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_easingType = m_cameraEasingType;
        }
    }
}

void SetupCameraRotatePopup::updateMoveCommandEasingRate()
{
    if (m_gameObject)
    {
        m_gameObject->m_easingRate = m_cameraEasingRate;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_easingRate = m_cameraEasingRate;
        }
    }
}

void SetupCameraRotatePopup::valuePopupClosed(ConfigureValuePopup* popup, float value)
{
    float clamped = cocos2d::CCString::createWithFormat("%.02f", value)->floatValue();
    if (clamped >= 20.0f)
        clamped = 20.0f;
    else if (clamped <= 0.1f)
        clamped = 0.1f;

    m_cameraEasingRate = clamped;
    updateMoveCommandEasingRate();
    updateEasingRateLabel();
}

SetupCameraRotatePopup::~SetupCameraRotatePopup()
{
}