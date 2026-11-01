#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class CheckpointGameObject;

class SetupCheckpointPopup : public SetupTriggerPopup {
public:
    static SetupCheckpointPopup* create(CheckpointGameObject* object, cocos2d::CCArray* objects);
    bool init(CheckpointGameObject* object, cocos2d::CCArray* objects);
    ~SetupCheckpointPopup();
};