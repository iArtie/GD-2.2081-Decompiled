#include "Popups/Setup/SetupKeyframePopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"
#include "Layers/LevelEditorLayer.h"
#include "Editor/EditorUI.h"
#include "Cocos/CCMenuItemToggler.h"

void SetupKeyframePopup::onClose(cocos2d::CCObject* sender)
{
    if (m_groupIDChanged && m_editorLayer)
    {
        int targetGroupID = m_gameObject->m_targetGroupID;
        cocos2d::CCObject* group = m_editorLayer->m_keyframeGroups->objectForKey(
            std::to_string(m_gameObject->m_nTag));

        if (group)
        {
            cocos2d::CCArray* objects = (cocos2d::CCArray*)group;
            for (unsigned int i = 0; i < objects->count(); i++)
            {
                EffectGameObject* obj = (EffectGameObject*)objects->objectAtIndex(i);
                obj->m_targetGroupID = targetGroupID;
                obj->m_keyframeGroupUpdated = true;
            }
        }
    }
    SetupTriggerPopup::onClose(sender);
}

void SetupKeyframePopup::onCustomButton(cocos2d::CCObject* sender)
{
    std::unordered_set<int> groupIDs;

    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            groupIDs.insert(obj->m_targetGroupID);
        }
    }

    int tag = sender->getTag();

    if (tag == 2)
    {
        refreshPreviewArt();
    }
    else if (tag == 4)
    {
        if (m_editorLayer)
        {
            for (int groupID : groupIDs)
                m_editorLayer->reverseKeyframeAnimationOrder(groupID);
        }
        onClose(nullptr);
    }
    else if (tag == 3)
    {
        m_editorLayer->m_editorUI->deselectAll();

        for (int groupID : groupIDs)
        {
            cocos2d::CCArray* objects = m_editorLayer->duplicateKeyframeAnimation(groupID);
            m_editorLayer->m_editorUI->selectObjects(objects, false);
        }
        onClose(nullptr);
    }
    else if (tag == 1 && m_editorLayer)
    {
        std::set<int> groupIDSet;
        if (m_gameObjects)
        {
            for (unsigned int i = 0; i < m_gameObjects->count(); i++)
            {
                EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
                groupIDSet.insert(obj->m_targetGroupID);
            }
        }

        m_editorLayer->m_editorUI->deselectAll();

        for (int groupID : groupIDSet)
        {
            cocos2d::CCArray* objects = (cocos2d::CCArray*)m_editorLayer->m_keyframeGroups->objectForKey(
                std::to_string(groupID));
            m_editorLayer->m_editorUI->selectObjects(objects, false);
        }
        onClose(nullptr);
    }
}

void SetupKeyframePopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    float currentValue = getValue(536);
    float newValue;
    bool toggle1 = false;
    bool toggle2 = false;

    if ((int)currentValue == sender->getTag())
    {
        newValue = 0.0f;
    }
    else
    {
        int tag = sender->getTag();
        newValue = (float)tag;
        toggle1 = (tag == 1);
        toggle2 = (tag == 2);
    }

    updateValue(536, newValue);
    updateCustomToggleTrigger(1, toggle1);
    updateCustomToggleTrigger(2, toggle2);
}

void SetupKeyframePopup::onTimeMode(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    cocos2d::CCObject* value = new cocos2d::CCObject();
    value->m_uID = tag;
    value->autorelease();

    m_triggerValues->setObject(value, "timeMode");
    valueChanged(379, (float)tag);
    updateTimeModeButtons();
}

void SetupKeyframePopup::refreshPreviewArt()
{
    if (!m_editorLayer)
        return;

    std::set<int> groupIDs;

    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            groupIDs.insert(obj->m_targetGroupID);
        }
    }

    for (int groupID : groupIDs)
    {
        cocos2d::CCObject* group = m_editorLayer->m_keyframeGroups->objectForKey(std::to_string(groupID));
        if (group)
        {
            cocos2d::CCArray* objects = (cocos2d::CCArray*)group;
            for (unsigned int i = 0; i < objects->count(); i++)
            {
                EffectGameObject* obj = (EffectGameObject*)objects->objectAtIndex(i);
                obj->m_keyframeGroupUpdated = true;
            }
        }
    }
}

void SetupKeyframePopup::updateTimeModeButtons()
{
    int timeMode = (int)getValue(379);

    if (m_timeModeToggles)
    {
        for (unsigned int i = 0; i < m_timeModeToggles->count(); i++)
        {
            CCMenuItemToggler* toggler = (CCMenuItemToggler*)m_timeModeToggles->objectAtIndex(i);
            toggler->toggle(timeMode == toggler->getTag());
        }
    }
}

void SetupKeyframePopup::valueDidChange(int tag, float value)
{
    if (tag == 380)
    {
        refreshPreviewArt();
    }
    else if (tag == 51)
    {
        m_groupIDChanged = true;
    }
}

SetupKeyframePopup::~SetupKeyframePopup()
{
}