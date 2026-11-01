#pragma once

#include "cocos2d.h"
#include "SetupTriggerPopup.h"

class AdvancedFollowTriggerObject;

class SetupAdvFollowRetargetPopup : public SetupTriggerPopup {
public:
    static SetupAdvFollowRetargetPopup* create(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects);
    bool init(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects) override;

    void valueDidChange(int tag, float value) override;
    void updateDefaultTriggerValues() override;

    ~SetupAdvFollowRetargetPopup();

protected:
    cocos2d::CCDictionary* m_inputNodes = nullptr;
};