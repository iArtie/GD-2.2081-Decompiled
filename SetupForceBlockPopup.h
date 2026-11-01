#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class ForceBlockGameObject;

class SetupForceBlockPopup : public SetupTriggerPopup {
public:
    static SetupForceBlockPopup* create(ForceBlockGameObject* object, cocos2d::CCArray* objects);
    bool init(ForceBlockGameObject* object, cocos2d::CCArray* objects);

    void valueDidChange(int tag, float value);

    ~SetupForceBlockPopup();
};