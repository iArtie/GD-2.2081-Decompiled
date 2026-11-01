#include "SetupTimeWarpPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCLabelBMFont.h"
#include "InfoAlertButton.h"
#include "Slider.h"
#include "SliderThumb.h"

SetupTimeWarpPopup* SetupTimeWarpPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupTimeWarpPopup* instance = new SetupTimeWarpPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupTimeWarpPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    float timeWarpMod = -1.0f;
    m_timeWarpMod = 1.0f;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
        float value = obj->m_timeWarpTimeMod;

        if (timeWarpMod == -1.0f)
            timeWarpMod = value;
        else if (timeWarpMod != value)
        {
            timeWarpMod = -2.0f;
            break;
        }
    }

    if (timeWarpMod > 0.0f)
        m_timeWarpMod = timeWarpMod;
}

bool SetupTimeWarpPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 340.0f, 200.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 18.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupTriggerPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Lets you <cg>warp</c> all time a given amount.\nI need to add more options\nbefore you can do Za Warudo...",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    m_timeWarpLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(m_timeWarpLabel);
    m_timeWarpLabel->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 20.0f);
    m_timeWarpLabel->setScale(0.8f);

    m_timeWarpSlider = Slider::create(this, menu_selector(SetupTimeWarpPopup::sliderChanged));
    addChild(m_timeWarpSlider);
    m_timeWarpSlider->setPosition(m_timeWarpLabel->getPosition() + cocos2d::CCPoint(0.0f, -23.0f));

    updateTimeWarpLabel();
    m_timeWarpSlider->setValue((m_timeWarpMod - 0.1f) / 1.9f);

    createMultiTriggerItemsDefaultVertical();

    return true;
}

void SetupTimeWarpPopup::sliderChanged(cocos2d::CCObject* sender)
{
    m_timeWarpMod = roundf(((SliderThumb*)sender)->getValue() * 1.9f + 0.1f) * 100.0f / 100.0f;
    updateTimeWarp();
    updateTimeWarpLabel();
}

void SetupTimeWarpPopup::updateTimeWarp()
{
    if (m_gameObject)
    {
        m_gameObject->m_timeWarpTimeMod = m_timeWarpMod;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_timeWarpTimeMod = m_timeWarpMod;
        }
    }
}

void SetupTimeWarpPopup::updateTimeWarpLabel()
{
    m_timeWarpLabel->setString(
        cocos2d::CCString::createWithFormat("TimeMod: %.02f", m_timeWarpMod)->getCString());
}

SetupTimeWarpPopup::~SetupTimeWarpPopup()
{
}