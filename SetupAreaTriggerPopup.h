#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EnterEffectObject;

class SetupAreaTriggerPopup : public SetupTriggerPopup {
public:
    static SetupAreaTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);
    bool init(EnterEffectObject* object, cocos2d::CCArray* objects);
    ~SetupAreaTriggerPopup();

    void determineStartValues();
    void updateDefaultTriggerValues();

protected:
    bool m_disableTextDelegate = false;
};