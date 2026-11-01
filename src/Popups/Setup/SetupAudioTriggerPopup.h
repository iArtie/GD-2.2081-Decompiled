#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

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