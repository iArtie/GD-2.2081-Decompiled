#include "Popups/Setup/SetupRotateCommandPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

SetupRotateCommandPopup* SetupRotateCommandPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupRotateCommandPopup* instance = new SetupRotateCommandPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupRotateCommandPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 440.0f, 310.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    addTitle("Setup Rotate Command");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Rotates</c> a group of objects a given amount of degrees.\n"
        "<cy>Target Group ID</c> is the group of objects being rotated.\n"
        "<cl>Center Group ID</c> defines the center to rotate around.\n"
        "<cp>Aim Mode</c> rotates the objects to face a given target object.\n"
        "<cr>Follow Mode</c> follows the rotation of a given target object. Use <co>Dynamic Mode</c> to update locations in"
        " real-time for the previous 2 options.\n"
        "Use <ca>Move Time</c> -1 for an infinite action.",
        1.0f);

    createMultiTriggerItemsDefaultHorizontal();

    cocos2d::CCPoint degreesPos = center + cocos2d::CCPoint(-100.0f, 100.0f);
    createValueControlAdvanced(
        68, "Degrees:", cocos2d::CCPoint(0.8f, -360.0f), 360.0f, &degreesPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)9001, 1, 2);

    cocos2d::CCPoint x360Pos = center + cocos2d::CCPoint(105.0f, 100.0f);
    createValueControlAdvanced(
        69, "x360:", cocos2d::CCPoint(0.8f, -20.0f), 20.0f, &x360Pos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)9001, 1, 0);

    cocos2d::CCPoint rotTargetPos = center + cocos2d::CCPoint(-80.0f, 90.0f);
    createValueControlAdvanced(
        401, "Rot Target ID:", cocos2d::CCPoint(0.8f, -100.0f), 100.0f, &rotTargetPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, (GJInputStyle)9002, 0, 0);

    cocos2d::CCPoint rotOffsetPos = center + cocos2d::CCPoint(80.0f, 90.0f);
    createValueControlAdvanced(
        402, "Rot Offset:", cocos2d::CCPoint(0.8f, -360.0f), 360.0f, &rotOffsetPos,
        Float, 1, 10, 0.0f, 0.0f, 1, 0, (GJInputStyle)9002, 0, 2);

    cocos2d::CCPoint p1Pos = center + cocos2d::CCPoint(170.0f, 120.0f);
    createToggleValueControl(138, "P1", cocos2d::CCPoint(1.0f, 0.0f), &p1Pos, 0, 0, 0.0f);

    cocos2d::CCPoint p2Pos = center + cocos2d::CCPoint(170.0f, 90.0f);
    createToggleValueControl(200, "P2", cocos2d::CCPoint(1.0f, 0.0f), &p2Pos, 0, 0, 0.0f);

    cocos2d::CCPoint aimPos = center + cocos2d::CCPoint(-180.0f, 48.0f);
    createToggleValueControl(100, "Aim\nMode", cocos2d::CCPoint(1.0f, 0.0f), &aimPos, 0, 0, 0.0f);

    cocos2d::CCPoint followPos = center + cocos2d::CCPoint(-80.0f, 48.0f);
    createToggleValueControl(394, "Follow\nMode", cocos2d::CCPoint(1.0f, 0.0f), &followPos, 0, 0, 0.0f);

    cocos2d::CCPoint dynamicPos = center + cocos2d::CCPoint(20.0f, 48.0f);
    createToggleValueControl(397, "Dynamic\nMode", cocos2d::CCPoint(1.0f, 0.0f), &dynamicPos, 0, 0, 0.0f);

    cocos2d::CCPoint lockPos = center + cocos2d::CCPoint(120.0f, 48.0f);
    createToggleValueControl(70, "Lock Obj\nRotation", cocos2d::CCPoint(1.0f, 0.0f), &lockPos, 0, 0, 0.0f);

    cocos2d::CCPoint moveTimePos = center + cocos2d::CCPoint(-100.0f, 0.0f);
    createValueControlAdvanced(
        10, "Move Time:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &moveTimePos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint easingPos = center + cocos2d::CCPoint(105.0f, 5.0f);
    createEasingControls(cocos2d::CCPoint(0.8f, 0.0f), 0.0f, &easingPos, 0);

    cocos2d::CCPoint easingRatePos = center + cocos2d::CCPoint(105.0f, 0.0f);
    createValueControlAdvanced(
        403, "Easing:", cocos2d::CCPoint(0.8f, 0.0f), 100.0f, &easingRatePos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, (GJInputStyle)9004, 0, 2);

    ButtonSprite* infBtn = ButtonSprite::create("Inf", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_04.png", 30.0f);
    infBtn->setScale(0.5f);
    CCMenuItemSpriteExtra* infItem = CCMenuItemSpriteExtra::create(
        infBtn, nullptr, menu_selector(SetupRotateCommandPopup::onInfiniteDuration));
    m_buttonMenu->addChild(infItem);
    infItem->setPosition(m_buttonMenu->convertToNodeSpace(center + cocos2d::CCPoint(-10.0f, 0.0f)));
    addObjectToGroup(infItem, 9004);
    addObjectToPage(infItem, 0);

    cocos2d::CCPoint targetGroupPos = center + cocos2d::CCPoint(-80.0f, -85.0f);
    createValueControlAdvanced(
        51, "Target Group ID:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &targetGroupPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint centerGroupPos = center + cocos2d::CCPoint(80.0f, -85.0f);
    createValueControlAdvanced(
        71, "Center Group ID:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &centerGroupPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint boundsBase(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint minXPos = boundsBase + cocos2d::CCPoint(-80.0f, 40.0f);
    createValueControlAdvanced(
        516, "MinX ID:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &minXPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 0);

    cocos2d::CCPoint maxXPos = boundsBase + cocos2d::CCPoint(80.0f, 40.0f);
    createValueControlAdvanced(
        518, "MaxX ID:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &maxXPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 0);

    cocos2d::CCPoint minYPos = boundsBase + cocos2d::CCPoint(-80.0f, -40.0f);
    createValueControlAdvanced(
        517, "MinY ID:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &minYPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 0);

    cocos2d::CCPoint maxYPos = boundsBase + cocos2d::CCPoint(80.0f, -40.0f);
    createValueControlAdvanced(
        519, "MaxY ID:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &maxYPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 0);

    createPageButtons(m_width * 0.5f + 15.0f, 1);
    goToPage(0, false);

    updateControlVisibility();

    postSetup();

    return true;
}

void SetupRotateCommandPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    sender->getTag();
}

void SetupRotateCommandPopup::onInfiniteDuration(cocos2d::CCObject* sender)
{
    updateValue(10, -1.0f);
}

float SetupRotateCommandPopup::triggerValueFromSliderValue(int tag, float value)
{
    if (tag == 68)
        return roundf(SetupTriggerPopup::triggerValueFromSliderValue(tag, value));

    return SetupTriggerPopup::triggerValueFromSliderValue(tag, value);
}

void SetupRotateCommandPopup::updateControlVisibility()
{
    int followMode;
    int nonFollowMode;

    if (getValue(100) == 0.0f)
    {
        followMode = getValue(394) != 0.0f;
        nonFollowMode = !followMode;
    }
    else
    {
        nonFollowMode = 0;
        followMode = 1;
    }

    toggleGroup(9002, followMode);
    toggleGroup(9001, nonFollowMode);

    bool dynamicFollow = followMode && (getValue(397) != 0.0f);
    toggleGroup(9003, !dynamicFollow);
    toggleGroup(9004, dynamicFollow);
}

void SetupRotateCommandPopup::valueDidChange(int tag, float value)
{
    if (tag == 200)
    {
        if (value == 0.0f)
            return;
        updateValue(138, 0.0f);
        updateValue(401, 0.0f);
    }
    else if (tag > 200)
    {
        if (tag == 394)
        {
            if (value != 0.0f)
                updateValue(100, 0.0f);
            updateControlVisibility();
        }
        else if (tag == 397)
        {
            updateControlVisibility();
        }
    }
    else
    {
        if (tag == 100)
        {
            if (value != 0.0f)
                updateValue(394, 0.0f);
            updateControlVisibility();
            togglePageArrows(value != 0.0f);
        }
        else if (tag == 138 && value != 0.0f)
        {
            updateValue(200, 0.0f);
            updateValue(401, 0.0f);
        }
    }
}

SetupRotateCommandPopup::~SetupRotateCommandPopup()
{
}