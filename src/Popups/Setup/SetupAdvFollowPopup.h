#pragma once

#include "Platform/cocos2d.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Editor/SelectPremadeDelegate.h"

class AdvancedFollowTriggerObject;
class SelectPremadeLayer;

class SetupAdvFollowPopup : public SetupTriggerPopup, public SelectPremadeDelegate {
public:
    static SetupAdvFollowPopup* create(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects);
    bool init(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects) override;

    void onCustomToggleTriggerValue(cocos2d::CCObject* sender) override;
    void valueDidChange(int tag, float value) override;
    void updateDefaultTriggerValues() override;

    void onMode(cocos2d::CCObject* sender);
    void onPremade(cocos2d::CCObject* sender);
    void selectPremadeClosed(SelectPremadeLayer* layer, int type) override;
    void updateMode(int mode);

    ~SetupAdvFollowPopup();

protected:
    cocos2d::CCDictionary* m_inputNodes = nullptr;
    AdvancedFollowTriggerObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
};