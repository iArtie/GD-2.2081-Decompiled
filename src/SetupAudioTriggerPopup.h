#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class SetupAudioTriggerPopup : public SetupTriggerPopup {
public:
    void addProxVolControls(int page);
    void onProxMode(cocos2d::CCObject* sender);
    void updateSpecialGroup();
    void valueDidChange(int tag, float value);
    ~SetupAudioTriggerPopup();

protected:
    cocos2d::CCArray* m_proximityButtons = nullptr;
};