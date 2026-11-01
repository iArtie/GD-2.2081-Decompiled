#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;
class LevelEditorLayer;
class CCMenuItemToggler;

class SetupKeyframePopup : public SetupTriggerPopup {
public:
    void onClose(cocos2d::CCObject* sender);
    void onCustomButton(cocos2d::CCObject* sender);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    void onTimeMode(cocos2d::CCObject* sender);
    void refreshPreviewArt();
    void updateTimeModeButtons();
    void valueDidChange(int tag, float value);

    ~SetupKeyframePopup();

protected:
    bool m_groupIDChanged = false;
    LevelEditorLayer* m_editorLayer = nullptr;
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_timeModeToggles = nullptr;
    cocos2d::CCDictionary* m_triggerValues = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
};