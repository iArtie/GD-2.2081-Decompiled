#include "SetupMoveCommandPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

SetupMoveCommandPopup* SetupMoveCommandPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupMoveCommandPopup* instance = new SetupMoveCommandPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupMoveCommandPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 440.0f, 310.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Setup Move Command");
    addCloseButton("OK");

    createMultiTriggerItemsCorner();

    addHelpButton(
        "Help",
        "<cg>Moves</c> a group of objects to another position.\n"
        "<cy>Easing</c> changes the method used.\n"
        "<cl>Target Mode</c> moves the group to the position of another group.\n"
        "<cp>Direction Mode</c> moves the group in the direction of another group. Use <cr>Dynamic Mode</c> to update locat"
        "ions in real-time for the 2 previous options.\n"
        "<co>Small Step</c> changes steps to their actual number, so 1 grid space is 30 instead of 10.",
        1.0f);

    cocos2d::CCPoint moveXPos(winSize.width * 0.5f - 100.0f, winSize.height * 0.5f + 100.0f);
    cocos2d::CCArray* moveXControl = createValueControlAdvanced(
        28, "Move X:", cocos2d::CCPoint(0.8f, -100.0f), 100.0f, &moveXPos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)28, 1, 0);
    addObjectsToGroup(moveXControl, 9001);

    cocos2d::CCPoint moveYPos(winSize.width * 0.5f + 105.0f, winSize.height * 0.5f + 100.0f);
    cocos2d::CCArray* moveYControl = createValueControlAdvanced(
        29, "Move Y:", cocos2d::CCPoint(0.8f, -100.0f), 100.0f, &moveYPos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)29, 1, 0);
    addObjectsToGroup(moveYControl, 9001);

    cocos2d::CCPoint modXPos(winSize.width * 0.5f - 100.0f, winSize.height * 0.5f + 100.0f);
    cocos2d::CCArray* modXControl = createValueControlAdvanced(
        143, "Mod X:", cocos2d::CCPoint(0.8f, -1.0f), 1.0f, &modXPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)143, 1, 3);
    addObjectsToGroup(modXControl, 9001);

    cocos2d::CCPoint modYPos(winSize.width * 0.5f + 105.0f, winSize.height * 0.5f + 100.0f);
    cocos2d::CCArray* modYControl = createValueControlAdvanced(
        144, "Mod Y:", cocos2d::CCPoint(0.8f, -1.0f), 1.0f, &modYPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)144, 1, 3);
    addObjectsToGroup(modYControl, 9001);

    addInfoLabel("Lock X:", 0.55f, cocos2d::CCPoint(winSize.width * 0.5f - 120.0f, winSize.height * 0.5f + 71.0f), false);

    cocos2d::CCPoint playerLockXPos(winSize.width * 0.5f - 180.0f, winSize.height * 0.5f + 45.0f);
    createToggleValueControl(58, "Player", cocos2d::CCPoint(1.0f, 0.0f), &playerLockXPos, 0, 0, 0.0f);

    cocos2d::CCPoint cameraLockXPos(winSize.width * 0.5f - 90.0f, winSize.height * 0.5f + 45.0f);
    createToggleValueControl(141, "Camera", cocos2d::CCPoint(1.0f, 0.0f), &cameraLockXPos, 0, 0, 0.0f);

    addInfoLabel("Lock Y:", 0.55f, cocos2d::CCPoint(winSize.width * 0.5f + 105.0f, winSize.height * 0.5f + 71.0f), false);

    cocos2d::CCPoint playerLockYPos(winSize.width * 0.5f + 35.0f, winSize.height * 0.5f + 45.0f);
    createToggleValueControl(59, "Player", cocos2d::CCPoint(1.0f, 0.0f), &playerLockYPos, 0, 0, 0.0f);

    cocos2d::CCPoint cameraLockYPos(winSize.width * 0.5f + 125.0f, winSize.height * 0.5f + 45.0f);
    createToggleValueControl(142, "Camera", cocos2d::CCPoint(1.0f, 0.0f), &cameraLockYPos, 0, 0, 0.0f);

    cocos2d::CCPoint centerGroupPos(winSize.width * 0.5f - 80.0f, winSize.height * 0.5f + 90.0f);
    createValueControlAdvanced(
        395, "Center Group ID:", cocos2d::CCPoint(0.8f, -100.0f), 100.0f, &centerGroupPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, (GJInputStyle)9004, 0, 0);

    cocos2d::CCPoint targetPosGroupPos(winSize.width * 0.5f + 80.0f, winSize.height * 0.5f + 90.0f);
    createValueControlAdvanced(
        71, "TargetPos Group ID:", cocos2d::CCPoint(0.8f, -100.0f), 100.0f, &targetPosGroupPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, (GJInputStyle)9004, 0, 0);

    cocos2d::CCPoint p1Pos(winSize.width * 0.5f + 168.0f, winSize.height * 0.5f + 105.0f);
    createToggleValueControl(138, "P1", cocos2d::CCPoint(1.0f, 0.0f), &p1Pos, 0, 0, 0.0f);

    cocos2d::CCPoint p2Pos(winSize.width * 0.5f + 168.0f, winSize.height * 0.5f + 75.0f);
    createToggleValueControl(200, "P2", cocos2d::CCPoint(1.0f, 0.0f), &p2Pos, 0, 0, 0.0f);

    cocos2d::CCPoint xOnlyPos(winSize.width * 0.5f - 70.0f, winSize.height * 0.5f + 50.0f);
    createCustomToggleValueControl(1, false, 1, "X Only", cocos2d::CCPoint(1.0f, 0.0f), &xOnlyPos, 0, 0);

    cocos2d::CCPoint yOnlyPos(winSize.width * 0.5f + 30.0f, winSize.height * 0.5f + 50.0f);
    createCustomToggleValueControl(2, false, 1, "Y Only", cocos2d::CCPoint(1.0f, 0.0f), &yOnlyPos, 0, 0);

    int targetMode = (int)getValue(101);
    updateCustomToggleTrigger(1, targetMode == 1);
    updateCustomToggleTrigger(2, targetMode == 2);

    cocos2d::CCPoint distancePos(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f);
    createValueControlAdvanced(
        396, "Distance:", cocos2d::CCPoint(0.8f, -100.0f), 100.0f, &distancePos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)9003, 1, 0);

    cocos2d::CCPoint targetModePos(winSize.width * 0.5f - 180.0f, winSize.height * 0.5f + 5.0f);
    createToggleValueControl(100, "Target\nMode", cocos2d::CCPoint(1.0f, 0.0f), &targetModePos, 0, 0, 0.0f);

    cocos2d::CCPoint directionModePos(winSize.width * 0.5f - 90.0f, winSize.height * 0.5f + 5.0f);
    createToggleValueControl(394, "Direction\nMode", cocos2d::CCPoint(1.0f, 0.0f), &directionModePos, 0, 0, 0.0f);

    cocos2d::CCPoint smallStepPos(winSize.width * 0.5f + 35.0f, winSize.height * 0.5f + 5.0f);
    createToggleValueControl(393, "Small\nStep", cocos2d::CCPoint(1.0f, 0.0f), &smallStepPos, 0, 0, 0.0f);

    cocos2d::CCPoint dynamicModePos(winSize.width * 0.5f + 125.0f, winSize.height * 0.5f + 5.0f);
    createToggleValueControl(397, "Dynamic\nMode", cocos2d::CCPoint(1.0f, 0.0f), &dynamicModePos, 0, 0, 0.0f);

    cocos2d::CCPoint moveTimePos(winSize.width * 0.5f - 100.0f, winSize.height * 0.5f - 35.0f);
    createValueControlAdvanced(
        10, "Move Time:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &moveTimePos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint easingPos(winSize.width * 0.5f + 105.0f, winSize.height * 0.5f - 30.0f);
    createEasingControls(cocos2d::CCPoint(0.8f, 0.0f), 0.0f, &easingPos, 0);

    cocos2d::CCPoint targetGroupPos(winSize.width * 0.5f + 125.0f, winSize.height * 0.5f - 120.0f);
    createValueControlAdvanced(
        51, "Target Group ID:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &targetGroupPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint silentPos(
        20.0f - (m_width * 0.5f - winSize.width * 0.5f) + 90.0f,
        20.0f - (m_height * 0.5f - winSize.height * 0.5f) + 30.0f);
    createToggleValueControl(544, "Silent", cocos2d::CCPoint(1.0f, 0.0f), &silentPos, 0, 0, 0.0f);

    m_smallStep = getValue(393) != 0.0f;
    updateControlVisibility();

    postSetup();

    return true;
}

void SetupMoveCommandPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if ((unsigned int)(tag - 1) <= 1)
    {
        float value = 0.0f;
        bool toggle1 = false;
        bool toggle2 = false;

        if (tag != (int)getValue(101))
        {
            value = (float)tag;
            toggle1 = (tag == 1);
            toggle2 = (tag == 2);
        }

        updateValue(101, value);
        updateCustomToggleTrigger(1, toggle1);
        updateCustomToggleTrigger(2, toggle2);
    }
}

float SetupMoveCommandPopup::triggerSliderValueFromValue(int tag, float value)
{
    if (m_smallStep || ((unsigned int)(tag - 28) > 1 && tag != 396))
        return SetupTriggerPopup::triggerSliderValueFromValue(tag, value);

    float result = floorf(value / 3.0f) / 200.0f + 0.5f;
    if (result >= 1.0f)
        return 1.0f;
    if (result <= 0.0f)
        return 0.0f;
    return result;
}

float SetupMoveCommandPopup::triggerValueFromSliderValue(int tag, float value)
{
    if (!m_smallStep && ((unsigned int)(tag - 28) <= 1 || tag == 396))
        return floorf((value - 0.5f) * 200.0f) * 3.0f;

    return SetupTriggerPopup::triggerValueFromSliderValue(tag, value);
}

void SetupMoveCommandPopup::updateControlVisibility()
{
    bool targetMode = getValue(100) != 0.0f;
    bool directionMode = getValue(394) != 0.0f;
    bool showDeltaControls = !directionMode && !targetMode;

    toggleGroup(9002, targetMode);
    toggleGroup(9003, directionMode);
    toggleGroup(9004, directionMode || targetMode);
    toggleGroup(9001, showDeltaControls);

    if (showDeltaControls)
    {
        int lockX = (getValue(58) == 0.0f) ? (getValue(141) != 0.0f) : 1;
        toggleGroup(28, !lockX);
        toggleGroup(143, lockX);

        int lockY = (getValue(59) == 0.0f) ? (getValue(142) != 0.0f) : 1;
        toggleGroup(29, !lockY);
        toggleGroup(144, lockY);
    }
}

void SetupMoveCommandPopup::updateInputNode(int tag, float value)
{
    if (!m_smallStep && ((unsigned int)(tag - 28) <= 1 || tag == 396))
        value /= 3.0f;

    SetupTriggerPopup::updateInputNode(tag, value);
}

void SetupMoveCommandPopup::updateInputValue(int tag, float* value)
{
    if (!m_smallStep && ((unsigned int)(tag - 28) <= 1 || tag == 396))
        *value *= 3.0f;
}

void SetupMoveCommandPopup::valueDidChange(int tag, float value)
{
    if (tag == 141)
    {
        if (value != 0.0f)
            updateValue(58, 0.0f);
        updateControlVisibility();
        return;
    }

    if (tag > 141)
    {
        if (tag == 393)
        {
            m_smallStep = value != 0.0f;
            updateValue(28, 0.0f);
            updateValue(29, 0.0f);
            updateValue(396, 0.0f);
            return;
        }
        if (tag > 393)
        {
            if (tag == 394)
            {
                if (value != 0.0f)
                    updateValue(100, 0.0f);
                updateControlVisibility();
                return;
            }
            if (tag == 544 && value != 0.0f)
            {
                updateValue(10, 0.0f);
                return;
            }
            return;
        }

        if (tag == 142)
        {
            if (value != 0.0f)
                updateValue(59, 0.0f);
            updateControlVisibility();
            return;
        }
        if (tag == 200 && value != 0.0f)
        {
            updateValue(138, 0.0f);
            updateValue(71, 0.0f);
            return;
        }
        return;
    }

    if (tag == 59)
    {
        if (value != 0.0f)
            updateValue(142, 0.0f);
        updateControlVisibility();
        return;
    }
    if (tag <= 59)
    {
        if (tag == 58)
        {
            if (value != 0.0f)
                updateValue(141, 0.0f);
            updateControlVisibility();
        }
        return;
    }
    if (tag == 100)
    {
        if (value != 0.0f)
            updateValue(394, 0.0f);
        updateControlVisibility();
        return;
    }
    if (tag == 138 && value != 0.0f)
    {
        updateValue(200, 0.0f);
        updateValue(71, 0.0f);
    }
}

SetupMoveCommandPopup::~SetupMoveCommandPopup()
{
}