#include "SetupItemEditTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "ItemTriggerGameObject.h"
#include "GameToolbox.h"

SetupItemEditTriggerPopup* SetupItemEditTriggerPopup::create(ItemTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupItemEditTriggerPopup* instance = new SetupItemEditTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupItemEditTriggerPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag > 1000)
    {
        int mode = tag - 1000;
        updateValue(478, (float)mode);
        updateCustomToggleTrigger(1001, mode == 1);
        updateCustomToggleTrigger(1002, mode == 2);
        updateCustomToggleTrigger(1003, mode == 3);
        updateFormulaLabel();
    }
    else if (tag > 100)
    {
        int newSign2 = tag - 100;
        bool t1 = false, t2 = false, t3 = false, t4 = false, t5 = false;
        float value = 0.0f;

        if ((int)getValue(477) != newSign2)
        {
            t1 = (newSign2 == 1);
            t2 = (newSign2 == 2);
            t3 = (newSign2 == 3);
            t4 = (newSign2 == 4);
            t5 = (newSign2 == 5);
            value = (float)newSign2;
        }

        updateValue(477, value);
        updateCustomToggleTrigger(101, t1);
        updateCustomToggleTrigger(102, t2);
        updateCustomToggleTrigger(103, t3);
        updateCustomToggleTrigger(104, t4);
        updateCustomToggleTrigger(105, t5);
        updateFormulaLabel();
    }
    else
    {
        bool t1 = false, t2 = false, t3 = false, t4 = false, t5 = false;
        float value = 0.0f;

        if (tag != (int)getValue(476))
        {
            t1 = (tag == 1);
            t2 = (tag == 2);
            t3 = (tag == 3);
            t4 = (tag == 4);
            t5 = (tag == 5);
            value = (float)tag;
        }

        updateValue(476, value);
        updateCustomToggleTrigger(1, t1);
        updateCustomToggleTrigger(2, t2);
        updateCustomToggleTrigger(3, t3);
        updateCustomToggleTrigger(4, t4);
        updateCustomToggleTrigger(5, t5);
        updateFormulaLabel();
    }
}

void SetupItemEditTriggerPopup::onOpButton(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    int value = (int)getValue(tag) + 1;

    if ((unsigned int)(tag - 485) <= 1)
    {
        if (value > 3)
            value = 0;
    }
    else if ((unsigned int)(tag - 578) <= 1)
    {
        if (value > 2)
            value = 0;
    }
    else
    {
        if (value > 4)
            value = 0;
    }

    if (tag == 481)
    {
        value = 1;
    }
    else if (value <= 2 && tag == 482)
    {
        value = 3;
    }

    updateValue(tag, (float)value);
    updateOpButton((CCMenuItemSpriteExtra*)sender, value, tag);
    updateFormulaLabel();
}

void SetupItemEditTriggerPopup::updateFormulaLabel()
{
    float itemSign = getValue(476);
    float itemSign2 = getValue(477);
    int targetID = (int)getValue(51);
    float itemID = getValue(80);
    float itemID2 = getValue(95);
    int mode = (int)getValue(478);
    float op = getValue(480);
    int compOp = (int)getValue(481);
    int compOp2 = (int)getValue(482);
    float round1 = getValue(485);
    float round2 = getValue(486);
    float extSign = getValue(578);
    float extSign2 = getValue(579);
    float targetValue = getValue(479);

    if (compOp <= 0)
        compOp = 1;
    if (compOp2 <= 2)
        compOp2 = 3;
    if (mode <= 0)
        mode = 1;

    if (targetID <= 0)
    {
        m_formulaLabel->setString("Select a target");
        m_formulaLabel->limitLabelWidth(400.0f, 0.8f, 0.0f);
        return;
    }

    std::string itemVar1 = getItemVarName((int)itemSign, (int)itemID);
    std::string itemVar2 = getItemVarName((int)itemSign2, (int)itemID2);
    std::string itemVar3 = getItemVarName(mode, targetID);

    bool hasFirst = ((int)itemSign != 0) && ((int)itemID > 0 || isSpecialItemType((int)itemSign));
    bool hasSecond = ((int)itemSign2 != 0) && ((int)itemID2 > 0 || isSpecialItemType((int)itemSign2));

    std::string formula = itemVar3 + " = ";

    bool useOp = ((int)op != 0);
    bool extSignActive = useOp && (int)extSign2 > 0;
    bool roundActive = useOp && (int)round2 > 0;

    if (extSignActive)
    {
        formula += signKeyToString((int)extSign2, false);
        formula += "(";
    }

    if (roundActive)
    {
        formula += roundKeyToString((int)round2, false);
        formula += "(";
    }
    else if (!useOp)
    {
        if ((int)extSign > 0)
        {
            formula += signKeyToString((int)extSign, false);
            formula += "(";
        }

        if ((int)round1 > 0)
        {
            formula += roundKeyToString((int)round1, false);
            formula += "(";
        }

        if (hasSecond)
        {
            formula += itemVar2;
            formula += " ";
            formula += opKeyToString(compOp);
            formula += " ";
            formula += itemVar1;
        }
        else
        {
            formula += itemVar1;
        }

        if ((int)round1 > 0)
            formula += ")";
        if ((int)extSign > 0)
            formula += ")";

        if ((int)round2 > 0)
            formula += ")";
        if ((int)extSign2 > 0)
            formula += ")";

        m_formulaLabel->setString(formula.c_str());
        m_formulaLabel->limitLabelWidth(400.0f, 0.8f, 0.0f);
        return;
    }

    if (hasSecond)
    {
        formula += itemVar2;
        formula += " ";
        formula += opKeyToString(compOp);
        formula += " ";
    }

    formula += itemVar1;

    if (useOp)
    {
        formula += " ";
        formula += opKeyToString((int)op);
        formula += " ";

        if (isOpValid(compOp2, targetValue))
        {
            formula += cocos2d::CCString::createWithFormat("%.03f", targetValue)->getCString();
        }
    }

    if (roundActive)
        formula += ")";
    if (extSignActive)
        formula += ")";

    m_formulaLabel->setString(formula.c_str());
    m_formulaLabel->limitLabelWidth(400.0f, 0.8f, 0.0f);
}

void SetupItemEditTriggerPopup::valueDidChange(int tag, float value)
{
    if (tag == 80 || tag == 51)
        updateFormulaLabel();
    else if (tag == 95 || tag == 479)
        updateFormulaLabel();
}

SetupItemEditTriggerPopup::~SetupItemEditTriggerPopup()
{
}