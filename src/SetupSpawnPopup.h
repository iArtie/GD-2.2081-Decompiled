#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupSpawnPopup : public SetupTriggerPopup {
public:
    static SetupSpawnPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void onAddRemap(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onDeleteRemap(cocos2d::CCObject* sender);
    void onPasteRemap(cocos2d::CCObject* sender);
    void onSelectRemap(cocos2d::CCObject* sender);
    void queueUpdateButtons();
    void updateRemapButtons(float dt);

    ~SetupSpawnPopup();

protected:
    bool m_isBusy = false;
    int m_remapOriginalID = 0;
    int m_remapNewID = 0;
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    std::vector<std::pair<int, int>> m_remapGroups;
    cocos2d::CCArray* m_remapButtons = nullptr;
};