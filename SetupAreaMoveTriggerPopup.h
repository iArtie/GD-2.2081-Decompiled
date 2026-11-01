#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupAreaTriggerPopup.h"

class EnterEffectObject;
class ButtonSprite;

class SetupAreaMoveTriggerPopup : public SetupAreaTriggerPopup {
public:
    static SetupAreaMoveTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);
    bool init(EnterEffectObject* object, cocos2d::CCArray* objects) override;

    void addAreaDefaultControls(int objectID);
    void onMode(cocos2d::CCObject* sender);
    void onNextFreeEffectID(cocos2d::CCObject* sender);
    void onSpecialTarget(cocos2d::CCObject* sender);
    float triggerSliderValueFromValue(int tag, float value) override;
    float triggerValueFromSliderValue(int tag, float value) override;
    void updateDefaultTriggerValues() override;
    void updateEnterTargetIDState();
    void updateInputNode(int tag, float value) override;
    void updateInputValue(int tag, float* value) override;

    ~SetupAreaMoveTriggerPopup();

protected:
    cocos2d::CCArray* m_targetButtons = nullptr;
    cocos2d::CCDictionary* m_triggerValues = nullptr;
    cocos2d::CCArray* m_modeButtons = nullptr;
};