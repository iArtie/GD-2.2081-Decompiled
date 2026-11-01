#include "Popups/Setup/SetupCollisionTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"
#include "Cocos/CCTextInputNode.h"

SetupCollisionTriggerPopup* SetupCollisionTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupCollisionTriggerPopup* instance = new SetupCollisionTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupCollisionTriggerPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_activateGroup = true;
    m_triggerOnExit = true;
    m_targetP1 = true;
    m_targetP2 = true;
    m_targetPP = true;
    m_blockAID = -1;
    m_blockBID = -1;
    m_targetID = -1;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!obj->m_activateGroup)
            m_activateGroup = false;
        if (!obj->m_triggerOnExit)
            m_triggerOnExit = false;
        if (!obj->m_targetPlayer1)
            m_targetP1 = false;
        if (!obj->m_targetPlayer2)
            m_targetP2 = false;
        if (!obj->m_followCPP)
            m_targetPP = false;

        if (m_blockAID == -1)
            m_blockAID = obj->m_itemID;
        else if (m_blockAID != obj->m_itemID)
            m_blockAID = 0;

        if (m_blockBID == -1)
            m_blockBID = obj->m_itemID2;
        else if (m_blockBID != obj->m_itemID2)
            m_blockBID = 0;

        if (m_targetID == -1)
            m_targetID = obj->m_targetGroupID;
        else if (m_targetID != obj->m_targetGroupID)
            m_targetID = 0;
    }
}

void SetupCollisionTriggerPopup::onActivateOnExit(cocos2d::CCObject* sender)
{
    m_triggerOnExit = !m_triggerOnExit;

    if (m_gameObject)
    {
        m_gameObject->m_triggerOnExit = m_triggerOnExit;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_triggerOnExit = m_triggerOnExit;
        }
    }
}

void SetupCollisionTriggerPopup::onClose(cocos2d::CCObject* sender)
{
    m_blockAInput->m_delegate = nullptr;
    m_blockAInput->onClickTrackNode(false);
    m_blockBInput->m_delegate = nullptr;
    m_blockBInput->onClickTrackNode(false);
    m_targetIDInput->m_delegate = nullptr;
    m_targetIDInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupCollisionTriggerPopup::onEnableGroup(cocos2d::CCObject* sender)
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

void SetupCollisionTriggerPopup::onItemID2Arrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_blockBID -= 1;
    else
        m_blockBID += 1;

    updateItemID2();
    updateItemID2InputLabel();
}

void SetupCollisionTriggerPopup::onItemIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_blockAID -= 1;
    else
        m_blockAID += 1;

    updateItemID();
    updateItemIDInputLabel();
}

void SetupCollisionTriggerPopup::onTargetIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_targetID -= 1;
    else
        m_targetID += 1;

    updateTargetID();
    updateTargetIDInputLabel();
    updateEditorLabel();
}

void SetupCollisionTriggerPopup::onTargetP1(cocos2d::CCObject* sender)
{
    m_targetP1 = !m_targetP1;

    if (m_gameObject)
    {
        m_gameObject->m_targetPlayer1 = m_targetP1;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_targetPlayer1 = m_targetP1;
        }
    }

    updateItemIDInputLabel();
    m_blockAInput->setTouchEnabled(!m_targetP2 && !m_targetP1);
}

void SetupCollisionTriggerPopup::onTargetP2(cocos2d::CCObject* sender)
{
    m_targetP2 = !m_targetP2;

    if (m_gameObject)
    {
        m_gameObject->m_targetPlayer2 = m_targetP2;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_targetPlayer2 = m_targetP2;
        }
    }

    updateItemIDInputLabel();
    m_blockAInput->setTouchEnabled(!m_targetP2 && !m_targetP1);
}

void SetupCollisionTriggerPopup::onTargetPP(cocos2d::CCObject* sender)
{
    m_targetPP = !m_targetPP;

    if (m_gameObject)
    {
        m_gameObject->m_followCPP = m_targetPP;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_followCPP = m_targetPP;
        }
    }

    updateItemIDInputLabel();
    updateItemID2InputLabel();
    m_blockAInput->setTouchEnabled(!m_targetPP);
    m_blockBInput->setTouchEnabled(!m_targetPP);
}

void SetupCollisionTriggerPopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    if (node->getTag() == 1)
    {
        m_blockBID = atoi(node->getString().c_str());
        updateItemID2();
    }
    else if (node->getTag() == 2)
    {
        m_targetID = atoi(node->getString().c_str());
        updateTargetID();
        updateEditorLabel();
    }
    else
    {
        m_blockAID = atoi(node->getString().c_str());
        updateItemID();
    }
}

void SetupCollisionTriggerPopup::updateItemID()
{
    if (m_blockAID < 0)
        m_blockAID = 0;
    if (m_blockAID > 9999)
        m_blockAID = 9999;

    if (m_gameObject)
    {
        m_gameObject->m_itemID = m_blockAID;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_itemID = m_blockAID;
        }
    }
}

void SetupCollisionTriggerPopup::updateItemID2()
{
    if (m_blockBID < 0)
        m_blockBID = 0;
    if (m_blockBID > 9999)
        m_blockBID = 9999;

    if (m_gameObject)
    {
        m_gameObject->m_itemID2 = m_blockBID;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_itemID2 = m_blockBID;
        }
    }
}

void SetupCollisionTriggerPopup::updateItemID2InputLabel()
{
    m_disableTextDelegate = true;

    std::string text = cocos2d::CCString::createWithFormat("%i", m_blockBID)->getCString();
    if (m_targetPP)
        text = "P2";

    m_blockBInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupCollisionTriggerPopup::updateItemIDInputLabel()
{
    m_disableTextDelegate = true;

    std::string text = cocos2d::CCString::createWithFormat("%i", m_blockAID)->getCString();
    if (m_targetPP)
        text = "P1";
    else if (m_targetP1 && m_targetP2)
        text = "P";
    else if (m_targetP1)
        text = "P1";
    else if (m_targetP2)
        text = "P2";

    m_blockAInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupCollisionTriggerPopup::updateTargetID()
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

void SetupCollisionTriggerPopup::updateTargetIDInputLabel()
{
    m_disableTextDelegate = true;

    std::string text = cocos2d::CCString::createWithFormat("%i", m_targetID)->getCString();
    m_targetIDInput->setString(text);
    m_disableTextDelegate = false;
}

SetupCollisionTriggerPopup::~SetupCollisionTriggerPopup()
{
}