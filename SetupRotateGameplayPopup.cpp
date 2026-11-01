#include "SetupRotateGameplayPopup.h"
#include "SetupTriggerPopup.h"
#include "RotateGameplayGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"

SetupRotateGameplayPopup* SetupRotateGameplayPopup::create(RotateGameplayGameObject* object, cocos2d::CCArray* objects)
{
    SetupRotateGameplayPopup* instance = new SetupRotateGameplayPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupRotateGameplayPopup::onPlusButton(cocos2d::CCObject* sender)
{
    std::unordered_set<int> usedChannels;
    int nextChannel = GameManager::sharedState()->m_levelEditorLayer->getNextFreeOrderChannel(&usedChannels);
    updateValue(173, (float)nextChannel);
}

void SetupRotateGameplayPopup::valueDidChange(int tag, float value)
{
    if (tag == 171 || tag == 169)
        toggleGroup(tag, value != 0.0f);
}

SetupRotateGameplayPopup::~SetupRotateGameplayPopup()
{
}