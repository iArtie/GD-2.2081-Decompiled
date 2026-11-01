#include "SetupInteractObjectPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"

SetupInteractObjectPopup* SetupInteractObjectPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupInteractObjectPopup* instance = new SetupInteractObjectPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupInteractObjectPopup::onClose(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    SetupTriggerPopup::onClose(sender);
}

void SetupInteractObjectPopup::onPlusButton(cocos2d::CCObject* sender)
{
    std::unordered_set<int> usedIDs;
    int nextID = GameManager::sharedState()->m_levelEditorLayer->getNextFreeItemID(&usedIDs);
    updateValue(80, (float)nextID);
}

void SetupInteractObjectPopup::updateItems()
{
    float value1 = getValue(381);
    toggleGroup(1, value1 != 0.0f);

    float value2 = getValue(382);
    toggleGroup(2, value2 != 0.0f);
}

void SetupInteractObjectPopup::valueDidChange(int tag, float value)
{
    if ((unsigned int)(tag - 381) <= 1)
    {
        updateItems();
    }
    else if (tag == 463)
    {
        updateValue(462, value);
    }
}

SetupInteractObjectPopup::~SetupInteractObjectPopup()
{
}