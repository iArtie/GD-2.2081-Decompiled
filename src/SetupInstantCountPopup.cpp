#include "SetupInstantCountPopup.h"
#include "SetupTriggerPopup.h"
#include "CountTriggerGameObject.h"
#include "EffectGameObject.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "CCTextInputNode.h"

SetupInstantCountPopup* SetupInstantCountPopup::create(CountTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupInstantCountPopup* instance = new SetupInstantCountPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupInstantCountPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_itemID = -1;
    m_targetID = -1;
    m_targetCount = 99999;
    m_mode = -1;
    m_activateGroup = true;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!obj->m_activateGroup)
            m_activateGroup = false;

        if (m_itemID == -1)
            m_itemID = obj->m_itemID;
        else if (m_itemID != obj->m_itemID)
            m_itemID = 0;

        if (m_targetID == -1)
            m_targetID = obj->m_targetGroupID;
        else if (m_targetID != obj->m_targetGroupID)
            m_targetID = 0;

        if (m_mode == -1)
            m_mode = obj->m_countMode;
        else if (m_mode != obj->m_countMode)
            m_mode = 0;

        if (m_targetCount == 99999)
            m_targetCount = obj->m_targetCount;
        else if (m_targetCount != obj->m_targetCount)
            m_targetCount = 0;
    }
}

void SetupInstantCountPopup::onClose(cocos2d::CCObject* sender)
{
    m_itemIDInput->m_delegate = nullptr;
    m_itemIDInput->onClickTrackNode(false);
    m_targetIDInput->m_delegate = nullptr;
    m_targetIDInput->onClickTrackNode(false);
    m_targetCountInput->m_delegate = nullptr;
    m_targetCountInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupInstantCountPopup::onCountType(cocos2d::CCObject* sender)
{
    m_mode = sender->getTag();

    m_equalsToggler->toggle(m_mode == 0);
    m_largerToggler->toggle(m_mode == 1);
    m_smallerToggler->toggle(m_mode == 2);

    if (m_gameObject)
    {
        m_gameObject->m_countMode = m_mode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_countMode = m_mode;
        }
    }
}

void SetupInstantCountPopup::onEnableGroup(cocos2d::CCObject* sender)
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

void SetupInstantCountPopup::onTargetCountArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_targetCount -= 1;
    else
        m_targetCount += 1;

    updateTargetCount();
    updateCountTextInputLabel();
}

void SetupInstantCountPopup::onTargetID2Arrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_targetID -= 1;
    else
        m_targetID += 1;

    updateTargetID();
    updateTargetIDInputLabel();
}

void SetupInstantCountPopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_itemID -= 1;
    else
        m_itemID += 1;

    updateItemID();
    updateEditorLabel();
    updateItemIDInputLabel();
}

void SetupInstantCountPopup::textChanged(CCTextInputNode* node)
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

void SetupInstantCountPopup::updateCountTextInputLabel()
{
    m_disableTextDelegate = true;
    m_targetCountInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_targetCount)->getCString());
    m_disableTextDelegate = false;
}

void SetupInstantCountPopup::updateItemID()
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

void SetupInstantCountPopup::updateItemIDInputLabel()
{
    m_disableTextDelegate = true;
    m_itemIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_itemID)->getCString());
    m_disableTextDelegate = false;
}

void SetupInstantCountPopup::updateTargetCount()
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

void SetupInstantCountPopup::updateTargetID()
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
}

void SetupInstantCountPopup::updateTargetIDInputLabel()
{
    m_disableTextDelegate = true;
    m_targetIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_targetID)->getCString());
    m_disableTextDelegate = false;
}

SetupInstantCountPopup::~SetupInstantCountPopup()
{
}