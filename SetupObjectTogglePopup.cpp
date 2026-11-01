#include "SetupObjectTogglePopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"

void SetupObjectTogglePopup::onClose(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    SetupTriggerPopup::onClose(sender);
}

void SetupObjectTogglePopup::valueDidChange(int tag, float value)
{
    if (tag == 56)
    {
        if (m_gameObject)
            m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

        if (!m_gameObjects)
            return;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->updateSpecialColor();
        }
    }
}

SetupObjectTogglePopup::~SetupObjectTogglePopup()
{
}