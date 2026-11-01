#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class SetupInstantCollisionTriggerPopup : public SetupTriggerPopup {
public:
    void updateDefaultTriggerValues();
    void updateSpecialNodes();
    void valueDidChange(int tag, float value);

protected:
    cocos2d::CCDictionary* m_inputNodes = nullptr;
};