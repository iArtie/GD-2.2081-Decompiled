#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;
class EnhancedGameObject;

class SetupAnimSettingsPopup : public SetupTriggerPopup {
public:
    static SetupAnimSettingsPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects) override;

    void onClose(cocos2d::CCObject* sender) override;
    void updateAnimSettings();

    ~SetupAnimSettingsPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
};