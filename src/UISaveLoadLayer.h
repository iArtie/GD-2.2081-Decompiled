#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class UIOptionsLayer;

class UISaveLoadLayer : public SetupTriggerPopup {
public:
    static UISaveLoadLayer* create(UIOptionsLayer* layer);
    bool init(UIOptionsLayer* layer) override;

    void onLoad(cocos2d::CCObject* sender);
    void onSave(cocos2d::CCObject* sender);

    ~UISaveLoadLayer();

protected:
    UIOptionsLayer* m_optionsLayer = nullptr;
};