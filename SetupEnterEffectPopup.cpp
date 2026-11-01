#include "SetupEnterEffectPopup.h"
#include "SetupTriggerPopup.h"
#include "EnterEffectObject.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "HSVWidgetPopup.h"
#include "CCMenuItemToggler.h"
#include "CCMenuItemSpriteExtra.h"

SetupEnterEffectPopup* SetupEnterEffectPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects, int id)
{
    SetupEnterEffectPopup* instance = new SetupEnterEffectPopup();
    if (instance->init(object, objects, id))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupEnterEffectPopup::determineStartValues()
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
    m_hsvValue = cocos2d::ccHSVValue(1.0f, 1.0f, 1.0f, 1.0f, false, false);
    bool hsvConflict = false;
    bool firstHsv = true;
    bool skipHsv = (m_objectID != 3021);

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (m_enterType < 0)
            m_enterType = obj->m_enterType;
        else if (m_enterType != obj->m_enterType)
            m_enterType = 0;

        if (!skipHsv)
        {
            if (firstHsv)
            {
                m_hsvValue = obj->m_hsvValue;
                firstHsv = false;
            }
            else if (m_hsvValue.h != obj->m_hsvValue.h ||
                     m_hsvValue.s != obj->m_hsvValue.s ||
                     m_hsvValue.v != obj->m_hsvValue.v ||
                     m_hsvValue.absoluteSaturation != obj->m_hsvValue.absoluteSaturation ||
                     m_hsvValue.absoluteBrightness != obj->m_hsvValue.absoluteBrightness)
            {
                m_hsvValue = cocos2d::ccHSVValue(0.0f, 0.0f, 0.0f, 0.0f, false, false);
                hsvConflict = true;
            }
        }
    }
}

void SetupEnterEffectPopup::onClose(cocos2d::CCObject* sender)
{
    if (m_objectID == 3021 && m_hsvChanged)
    {
        if (m_gameObject)
        {
            m_gameObject->m_hsvValue = m_hsvValue;
        }
        else if (m_gameObjects)
        {
            for (unsigned int i = 0; i < m_gameObjects->count(); i++)
            {
                EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
                obj->m_hsvValue = m_hsvValue;
            }
        }
    }
    SetupTriggerPopup::onClose(sender);
}

void SetupEnterEffectPopup::onEnterType(cocos2d::CCObject* sender)
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

void SetupEnterEffectPopup::onHSV(cocos2d::CCObject* sender)
{
    HSVWidgetPopup* popup = HSVWidgetPopup::create(m_hsvValue, this, "Set HSV");
    popup->show();
}

void SetupEnterEffectPopup::onNextFreeEnterChannel(cocos2d::CCObject* sender)
{
    std::unordered_set<int> usedChannels;
    int nextChannel = GameManager::sharedState()->m_levelEditorLayer->getNextFreeEnterChannel(&usedChannels);
    updateValue(344, (float)nextChannel);
}

void SetupEnterEffectPopup::onNextFreeEnterEffectID(cocos2d::CCObject* sender)
{
    std::unordered_set<int> usedIDs;
    int nextID = GameManager::sharedState()->m_levelEditorLayer->getNextFreeAreaEffectID(&usedIDs);
    updateValue(225, (float)nextID);
}

float SetupEnterEffectPopup::triggerSliderValueFromValue(int tag, float value)
{
    if (shouldModifyEnterValue(tag))
        value /= 3.0f;
    return SetupTriggerPopup::triggerSliderValueFromValue(tag, value);
}

float SetupEnterEffectPopup::triggerValueFromSliderValue(int tag, float value)
{
    float result = SetupTriggerPopup::triggerValueFromSliderValue(tag, value);
    if (shouldModifyEnterValue(tag))
        return floorf(result) * 3.0f;
    return result;
}

void SetupEnterEffectPopup::updateHSVButton()
{
    cocos2d::CCNode* normalImage = m_hsvButton->getNormalImage();
    const char* frameName;

    if (m_hsvValue.h == 1.0f && m_hsvValue.s == 1.0f && m_hsvValue.v == 1.0f &&
        m_hsvValue.absoluteSaturation == false && m_hsvValue.absoluteBrightness == false)
        frameName = "GJ_hsvBtn_001.png";
    else
        frameName = "GJ_hsv2Btn_001.png";

    normalImage->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName));
}

void SetupEnterEffectPopup::updateInputNode(int tag, float value)
{
    if (shouldModifyEnterValue(tag))
        value /= 3.0f;
    SetupTriggerPopup::updateInputNode(tag, value);
}

void SetupEnterEffectPopup::updateInputValue(int tag, float* value)
{
    if (shouldModifyEnterValue(tag))
        *value *= 3.0f;
}

void SetupEnterEffectPopup::valueDidChange(int tag, float value)
{
    if (tag == 66)
    {
        if (value == 0.0f)
            return;
        updateValue(65, 0.0f);
        return;
    }
    if (tag <= 66)
    {
        if (tag != 65 || value == 0.0f)
            return;
        updateValue(66, 0.0f);
        return;
    }
    if (tag == 241 || tag == 278)
    {
        toggleGroup(1, value == 0.0f);
        toggleGroup(2, value != 0.0f);
    }
}

SetupEnterEffectPopup::~SetupEnterEffectPopup()
{
}