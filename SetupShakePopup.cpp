#include "SetupShakePopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "InfoAlertButton.h"
#include "Slider.h"
#include "SliderThumb.h"

SetupShakePopup* SetupShakePopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupShakePopup* instance = new SetupShakePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupShakePopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    float duration = -99999.0f;
    float strength = -99999.0f;
    float interval = -99999.0f;
    int durationConflict = 0;
    int strengthConflict = 0;
    int intervalConflict = 0;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!durationConflict)
        {
            if (duration == -99999.0f)
                duration = obj->m_duration;
            else if (duration != obj->m_duration)
            {
                duration = -99999.0f;
                durationConflict = 1;
            }
        }

        if (!strengthConflict)
        {
            if (strength == -99999.0f)
                strength = obj->m_shakeStrength;
            else if (strength != obj->m_shakeStrength)
            {
                strength = -99999.0f;
                strengthConflict = 1;
            }
        }

        if (!intervalConflict)
        {
            if (interval == -99999.0f)
                interval = obj->m_shakeInterval;
            else if (interval != obj->m_shakeInterval)
            {
                interval = -99999.0f;
                intervalConflict = 1;
            }
        }
    }

    if (duration != -99999.0f)
        m_duration = duration;
    if (strength != -99999.0f)
        m_strength = strength;
    if (interval != -99999.0f)
        m_interval = interval;
}

bool SetupShakePopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 285.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.5f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 14.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupShakePopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "<cg>Shakes</c> the screen for a set duration.\n"
        "<cy>Strength</c> is how intense the shake is.\n"
        "<cl>Interval</c> is how often the screen shakes. Leaving this at 0 means that every frame shakes.\n"
        "Avoid overusing this effect as it can cause bad gameplay.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    createMultiTriggerItemsDefault();

    cocos2d::CCPoint basePos(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f);

    auto createTimingControl = [&](const char* label, cocos2d::CCPoint pos, float initialValue,
        float divider, int inputTag, int sliderTag, CCTextInputNode** outInput, Slider** outSlider)
    {
        cocos2d::CCLabelBMFont* lbl = cocos2d::CCLabelBMFont::create(label, "bigFont.fnt");
        lbl->setScale(0.7f);
        lbl->setPosition(pos);
        lbl->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        m_mainLayer->addChild(lbl);

        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
        bg->setColor(cocos2d::ccColor3B(0, 0, 0));
        bg->setOpacity(100);
        bg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(pos + cocos2d::CCPoint(45.0f, 0.0f));

        *outInput = CCTextInputNode::create(
            50.0f, 40.0f, initialValue == -99999.0f ? "Mixed" : "Num", "Thonburi");
        m_mainLayer->addChild(*outInput, 20);
        (*outInput)->setPosition(bg->getPosition());
        (*outInput)->setMaxLabelScale(0.6f);
        (*outInput)->setMaxLabelWidth(200.0f);
        (*outInput)->m_textField->setVisible(false);
        (*outInput)->m_maxLabelLength = 4;
        (*outInput)->setAllowedChars("0123456789.");
        (*outInput)->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
        (*outInput)->setLabelPlaceholderScale(0.5f);
        (*outInput)->m_numberInput = true;
        (*outInput)->m_delegate = this;
        (*outInput)->setTag(inputTag);

        *outSlider = Slider::create(this, menu_selector(SetupShakePopup::sliderChanged), 0, 0.8f);
        (*outSlider)->setBarVisibility(false);
        m_mainLayer->addChild(*outSlider);
        (*outSlider)->setPosition(pos + cocos2d::CCPoint(0.0f, -32.0f));
        (*outSlider)->getThumb()->setTag(sliderTag);

        float value = 1.0f;
        float computed = initialValue / divider;
        if (computed >= 1.0f)
            computed = value;
        else if (computed <= 0.0f)
            computed = 0.0f;
        (*outSlider)->setValue(computed);
    };

    createTimingControl("Strength: ", basePos, m_strength, 5.0f, 6, 4, &m_strengthInput, &m_strengthSlider);
    createTimingControl("Interval: ", basePos + cocos2d::CCPoint(0.0f, -60.0f), m_interval, 0.1f, 8, 7, &m_intervalInput, &m_intervalSlider);
    createTimingControl("Duration: ", basePos + cocos2d::CCPoint(0.0f, -120.0f), m_duration, 10.0f, 5, 3, &m_durationInput, &m_durationSlider);

    updateShakeLabel(false);
    updateDurLabel(false);
    updateIntervalLabel(false);

    m_disableTextDelegate = false;

    return true;
}

void SetupShakePopup::onClose(cocos2d::CCObject* sender)
{
    m_strengthInput->m_delegate = nullptr;
    m_strengthInput->onClickTrackNode(false);
    m_durationInput->m_delegate = nullptr;
    m_durationInput->onClickTrackNode(false);
    m_intervalInput->m_delegate = nullptr;
    m_intervalInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupShakePopup::sliderChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();

    if (sender->getTag() == 3)
    {
        m_duration = value + value;
        updateDuration();
        updateDurLabel(true);
    }
    else if (sender->getTag() == 7)
    {
        m_interval = value * 0.1f;
        updateInterval();
        updateIntervalLabel(true);
    }
    else
    {
        m_strength = value * 5.0f;
        updateShake();
        updateShakeLabel(true);
    }
}

void SetupShakePopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    if (node->getTag() == 5)
    {
        float value = cocos2d::CCString::createWithFormat(
            "%.02f", atof(node->getString().c_str()))->floatValue();
        if (value <= 0.0f) value = 0.0f;
        else if (value >= 10.0f) value = 10.0f;
        m_duration = value;
        updateDuration();
        float sliderValue = m_duration / 10.0f;
        m_durationSlider->setValue(sliderValue >= 1.0f ? 1.0f : (sliderValue <= 0.0f ? 0.0f : sliderValue));
    }
    else if (node->getTag() == 8)
    {
        float value = cocos2d::CCString::createWithFormat(
            "%.02f", atof(node->getString().c_str()))->floatValue();
        if (value <= 0.0f) value = 0.0f;
        else if (value >= 0.2f) value = 0.2f;
        m_interval = value;
        updateInterval();
        float sliderValue = m_duration / 0.1f;
        m_intervalSlider->setValue(sliderValue >= 1.0f ? 1.0f : (sliderValue <= 0.0f ? 0.0f : sliderValue));
    }
    else
    {
        float value = cocos2d::CCString::createWithFormat(
            "%.02f", atof(node->getString().c_str()))->floatValue();
        if (value <= 0.0f) value = 0.0f;
        else if (value >= 100.0f) value = 100.0f;
        m_strength = value;
        updateShake();
        float sliderValue = m_strength / 5.0f;
        m_strengthSlider->setValue(sliderValue >= 1.0f ? 1.0f : (sliderValue <= 0.0f ? 0.0f : sliderValue));
    }
}

void SetupShakePopup::updateDurLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_duration == -99999.0f)
        text = "Mixed";
    else if (!decimals && m_duration - (float)(int)m_duration == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_duration)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_duration)->getCString();

    m_durationInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupShakePopup::updateDuration()
{
    if (m_gameObject)
    {
        m_gameObject->m_duration = m_duration;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_duration = m_duration;
        }
    }
}

void SetupShakePopup::updateInterval()
{
    if (m_gameObject)
    {
        m_gameObject->m_shakeInterval = m_interval;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_shakeInterval = m_interval;
        }
    }
}

void SetupShakePopup::updateIntervalLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_interval == -99999.0f)
        text = "Mixed";
    else if (!decimals && m_interval - (float)(int)m_interval == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_interval)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_interval)->getCString();

    m_intervalInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupShakePopup::updateShake()
{
    if (m_gameObject)
    {
        m_gameObject->m_shakeStrength = m_strength;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_shakeStrength = m_strength;
        }
    }
}

void SetupShakePopup::updateShakeLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_strength == -99999.0f)
        text = "Mixed";
    else if (!decimals && m_strength - (float)(int)m_strength == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_strength)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_strength)->getCString();

    m_strengthInput->setString(text);
    m_disableTextDelegate = false;
}

SetupShakePopup::~SetupShakePopup()
{
}