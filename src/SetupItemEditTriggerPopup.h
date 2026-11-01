#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

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