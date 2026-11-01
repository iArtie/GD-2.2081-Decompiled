#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class ItemTriggerGameObject;

class SetupPersistentItemTriggerPopup : public SetupTriggerPopup {
public:
    static SetupPersistentItemTriggerPopup* create(ItemTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(ItemTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupPersistentItemTriggerPopup();
};