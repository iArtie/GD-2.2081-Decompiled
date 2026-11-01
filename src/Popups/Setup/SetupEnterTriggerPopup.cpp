#include "Popups/Setup/SetupEnterTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EnterEffectObject.h"
#include "Objects/EffectGameObject.h"
#include "Utils/GameToolbox.h"
#include "Cocos/CCMenuItemToggler.h"

SetupEnterTriggerPopup* SetupEnterTriggerPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects)
{
    SetupEnterTriggerPopup* instance = new SetupEnterTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupEnterTriggerPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_enterType = -1;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
        int type = obj->m_enterType;

        if (m_enterType < 0)
            m_enterType = type;
        else if (m_enterType != type)
            m_enterType = 0;
    }
}

bool SetupEnterTriggerPopup::init(EnterEffectObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 330.0f, 200.0f, 1))
        return false;

    m_disableTextDelegate = true;
    determineStartValues();

    addTitle("Enter Effect Trigger");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Target Enter Channel</c> lets you target only objects with a set Enter Channel.\n"
        "<cy>Enter and Exit Only</c> makes the effect only apply upon entering or exiting respectively.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint base(winSize.width * 0.5f, winSize.height * 0.5f - 25.0f);

    bool enterOnly = (m_enterType == 1);
    cocos2d::CCPoint enterPos = base + cocos2d::CCPoint(-50.0f, 0.0f);
    m_enterOnlyToggler = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Enter\nOnly", menu_selector(SetupEnterTriggerPopup::onEnterType), 0, enterOnly,
        m_buttonMenu, enterPos, this, m_mainLayer, 0.7f, 0.35f, 110.0f,
        cocos2d::CCPoint(6.0f, 0.0f), "bigFont.fnt", false, 0, false);

    bool exitOnly = (m_enterType == 2);
    cocos2d::CCPoint exitPos = base + cocos2d::CCPoint(50.0f, 0.0f);
    m_exitOnlyToggler = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Exit\nOnly", menu_selector(SetupEnterTriggerPopup::onEnterType), 0, exitOnly,
        m_buttonMenu, exitPos, this, m_mainLayer, 0.7f, 0.35f, 110.0f,
        cocos2d::CCPoint(6.0f, 0.0f), "bigFont.fnt", false, 0, false);

    m_enterOnlyToggler->m_notClickable = true;
    m_enterOnlyToggler->setTag(1);
    m_exitOnlyToggler->m_notClickable = true;
    m_exitOnlyToggler->setTag(2);

    cocos2d::CCPoint channelPos = base + cocos2d::CCPoint(0.0f, 50.0f);
    createValueControlAdvanced(
        344, "Target Enter Channel:", cocos2d::CCPoint(0.7f, 0.0f), 0.0f, &channelPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    updateDefaultTriggerValues();

    m_disableTextDelegate = false;

    return true;
}

void SetupEnterTriggerPopup::onEnterType(cocos2d::CCObject* sender)
{
    bool enterOnly;
    if (sender->getTag() == m_enterType)
    {
        m_enterType = 0;
        enterOnly = false;
    }
    else
    {
        m_enterType = sender->getTag();
        enterOnly = (m_enterType == 1);
    }

    m_enterOnlyToggler->toggle(enterOnly);
    m_exitOnlyToggler->toggle(m_enterType == 2);

    if (m_gameObject)
    {
        m_gameObject->m_enterType = m_enterType;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_enterType = m_enterType;
        }
    }
}

SetupEnterTriggerPopup::~SetupEnterTriggerPopup()
{
}