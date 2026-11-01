#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupAreaAnimTriggerPopup : public SetupTriggerPopup {
public:
    static SetupAreaAnimTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects) override;

    void valueDidChange(int tag, float value) override;
    void updateDefaultTriggerValues() override;

    void onDeactivateAnimValue(cocos2d::CCObject* sender);
    void updateTargetIDLabel();

    ~SetupAreaAnimTriggerPopup();

protected:
    cocos2d::CCDictionary* m_inputLabels = nullptr;
    cocos2d::CCDictionary* m_inputNodes = nullptr;
};