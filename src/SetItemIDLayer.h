#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;
class LabelGameObject;

class SetItemIDLayer : public SetupTriggerPopup {
public:
    static SetItemIDLayer* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects) override;

    void onClose(cocos2d::CCObject* sender) override;
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender) override;
    void valueDidChange(int tag, float value) override;

    void updateEditorLabel();

    ~SetItemIDLayer();
};