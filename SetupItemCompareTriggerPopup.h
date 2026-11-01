#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class CCLabelBMFont;
class CCMenuItemSpriteExtra;

class SetupItemCompareTriggerPopup : public SetupTriggerPopup {
public:
    void onOpButton(cocos2d::CCObject* sender);
    void updateFormulaLabel();
    void valueDidChange(int tag, float value);

    ~SetupItemCompareTriggerPopup();

protected:
    CCLabelBMFont* m_formulaLabel = nullptr;
};