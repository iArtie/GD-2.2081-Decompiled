#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class ItemTriggerGameObject;
class CCLabelBMFont;
class CCMenuItemSpriteExtra;

class SetupItemEditTriggerPopup : public SetupTriggerPopup {
public:
    static SetupItemEditTriggerPopup* create(ItemTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(ItemTriggerGameObject* object, cocos2d::CCArray* objects);

    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    void onOpButton(cocos2d::CCObject* sender);
    void updateFormulaLabel();
    void valueDidChange(int tag, float value);

    ~SetupItemEditTriggerPopup();

protected:
    CCLabelBMFont* m_formulaLabel = nullptr;
};