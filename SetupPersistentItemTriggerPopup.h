#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class ItemTriggerGameObject;

class SetupPersistentItemTriggerPopup : public SetupTriggerPopup {
public:
    static SetupPersistentItemTriggerPopup* create(ItemTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(ItemTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupPersistentItemTriggerPopup();
};