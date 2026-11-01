#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

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