#include "SetupItemCompareTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "GameToolbox.h"

void SetupItemCompareTriggerPopup::onOpButton(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    int value = (int)getValue(tag) + 1;
    float finalValue;

    if (tag == 482)
    {
        if (value > 5)
        {
            finalValue = 0.0f;
            value = 0;
        }
        else
        {
            finalValue = (float)value;
        }
    }
    else if ((unsigned int)(tag - 485) <= 1)
    {
        if (value > 3)
            value = (unsigned int)(tag - 480) <= 1;
        finalValue = (float)value;
    }
    else if ((unsigned int)(tag - 578) <= 1)
    {
        if (value > 2)
            value = (unsigned int)(tag - 480) <= 1;
        finalValue = (float)value;
    }
    else
    {
        if (value > 4)
            value = (unsigned int)(tag - 480) <= 1;
        else if ((unsigned int)(tag - 480) <= 1 && value <= 0)
        {
            finalValue = 1.0f;
            value = 1;
        }
        else
        {
            finalValue = (float)value;
        }
    }

    updateValue(tag, finalValue);
    updateOpButton((CCMenuItemSpriteExtra*)sender, value, tag);
    updateFormulaLabel();
}

void SetupItemCompareTriggerPopup::updateFormulaLabel()
{
    int itemID = (int)getValue(80);
    int itemID2 = (int)getValue(95);
    int itemSign = (int)getValue(476);
    int itemSign2 = (int)getValue(477);
    int op1 = (int)getValue(480);
    int op2 = (int)getValue(481);
    int compare = (int)getValue(482);
    int round1 = (int)getValue(485);
    int round2 = (int)getValue(486);
    int extSign = (int)getValue(578);
    int extSign2 = (int)getValue(579);

    if (op1 <= 0)
        op1 = 3;
    if (op2 <= 0)
        op2 = 3;

    float targetValue1 = getValue(479);
    float targetValue2 = getValue(483);

    std::string itemVar1 = getItemVarName(itemSign <= 0 ? 1 : itemSign, itemID);
    std::string itemVar2 = getItemVarName(itemSign2, itemID2);

    bool hasSecondItem = (itemSign2 != 0) && (itemID2 > 0 || isSpecialItemType(itemSign2));

    std::string formula;

    if (extSign > 0)
    {
        formula += signKeyToString(extSign, false);
        formula += "(";
    }

    if (round1 > 0)
    {
        formula += roundKeyToString(round1, false);
        formula += "(";
    }

    formula += itemVar1;

    if (isOpValid(op1, targetValue1))
    {
        formula += " ";
        formula += opKeyToString(op1);
        formula += " ";
        formula += cocos2d::CCString::createWithFormat("%.03f", targetValue1)->getCString();
    }

    if (round1 > 0)
        formula += ")";
    if (extSign > 0)
        formula += ")";

    formula += " ";
    formula += compKeyToString(compare);
    formula += " ";

    if (extSign2 > 0)
    {
        formula += signKeyToString(extSign2, false);
        formula += "(";
    }

    if (round2 > 0)
    {
        formula += roundKeyToString(round2, false);
        formula += "(";
    }

    if (hasSecondItem)
    {
        formula += itemVar2;

        if (isOpValid(op2, targetValue2))
        {
            formula += " ";
            formula += opKeyToString(op2);
            formula += " ";
            formula += cocos2d::CCString::createWithFormat("%.03f", targetValue2)->getCString();
        }
    }
    else
    {
        formula += cocos2d::CCString::createWithFormat("%.03f", targetValue2)->getCString();
    }

    if (round2 > 0)
        formula += ")";
    if (extSign2 > 0)
        formula += ")";

    m_formulaLabel->setString(formula.c_str());
    m_formulaLabel->limitLabelWidth(230.0f, 0.7f, 0.0f);
}

void SetupItemCompareTriggerPopup::valueDidChange(int tag, float value)
{
    if (tag == 95)
    {
        updateFormulaLabel();
        return;
    }
    if (tag <= 95)
    {
        if (tag == 80)
            updateFormulaLabel();
        return;
    }
    if (tag == 479 || tag == 483)
        updateFormulaLabel();
}

SetupItemCompareTriggerPopup::~SetupItemCompareTriggerPopup()
{
}