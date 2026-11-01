#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class SetupInstantCollisionTriggerPopup : public SetupTriggerPopup {
public:
    void updateDefaultTriggerValues();
    void updateSpecialNodes();
    void valueDidChange(int tag, float value);

protected:
    cocos2d::CCDictionary* m_inputNodes = nullptr;
};