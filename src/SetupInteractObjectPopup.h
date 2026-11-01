#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupInteractObjectPopup : public SetupTriggerPopup {
public:
    static SetupInteractObjectPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void onClose(cocos2d::CCObject* sender);
    void onPlusButton(cocos2d::CCObject* sender);
    void updateItems();
    void valueDidChange(int tag, float value);

    ~SetupInteractObjectPopup();
};