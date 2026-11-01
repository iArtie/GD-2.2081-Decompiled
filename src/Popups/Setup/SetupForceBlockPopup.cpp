#include "Popups/Setup/SetupForceBlockPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/ForceBlockGameObject.h"

SetupForceBlockPopup* SetupForceBlockPopup::create(ForceBlockGameObject* object, cocos2d::CCArray* objects)
{
    SetupForceBlockPopup* instance = new SetupForceBlockPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupForceBlockPopup::init(ForceBlockGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 380.0f, 270.0f, 1))
        return false;

    preSetup();

    addTitle("Setup Force Block");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Force</c> is how much force is applied upon player collision.\n"
        "Blocks with the same <cy>Force ID</c> do not stack.\n"
        "<cl>Relative</c> makes force direction based on angle between player and object center.\n"
        "<cp>UseRange</c> lets you set a range to apply force from based on player position. Range calculation is based on block radius.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint forcePos(winSize.width * 0.5f, winSize.height * 0.5f + 80.0f);
    createValueControlAdvanced(
        149, "Force:", cocos2d::CCPoint(0.8f, 0.0f), 5.0f, &forcePos,
        (InputValueType)0, 1, 4, 0.0f, 0.0f, 0, 0, (GJInputStyle)149, 0, 3);

    cocos2d::CCPoint minForcePos(winSize.width * 0.5f - 80.0f, winSize.height * 0.5f + 80.0f);
    createValueControlAdvanced(
        526, "MinForce:", cocos2d::CCPoint(0.8f, 0.0f), 5.0f, &minForcePos,
        (InputValueType)0, 1, 4, 0.0f, 0.0f, 0, 0, (GJInputStyle)529, 0, 3);

    cocos2d::CCPoint maxForcePos(winSize.width * 0.5f + 80.0f, winSize.height * 0.5f + 80.0f);
    createValueControlAdvanced(
        527, "MaxForce:", cocos2d::CCPoint(0.8f, 0.0f), 5.0f, &maxForcePos,
        (InputValueType)0, 1, 4, 0.0f, 0.0f, 0, 0, (GJInputStyle)529, 0, 3);

    cocos2d::CCPoint relativePos(winSize.width * 0.5f - 80.0f, winSize.height * 0.5f + 15.0f);
    createToggleValueControl(528, "Relative", cocos2d::CCPoint(1.0f, 0.0f), &relativePos, 0, 0, 0.0f);

    cocos2d::CCPoint rangePos(winSize.width * 0.5f + 50.0f, winSize.height * 0.5f + 15.0f);
    createToggleValueControl(529, "Range", cocos2d::CCPoint(1.0f, 0.0f), &rangePos, 0, 0, 0.0f);

    cocos2d::CCPoint forceIDPos(winSize.width * 0.5f, winSize.height * 0.5f - 55.0f);
    createValueControlAdvanced(
        530, "ForceID", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &forceIDPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    float rangeValue = getValue(529);
    bool rangeEnabled = (rangeValue != 0.0f);
    toggleGroup(529, rangeEnabled);
    toggleGroup(149, !rangeEnabled);

    postSetup();

    return true;
}

void SetupForceBlockPopup::valueDidChange(int tag, float value)
{
    if (tag == 529)
    {
        toggleGroup(529, value != 0.0f);
        toggleGroup(149, value == 0.0f);
    }
}

SetupForceBlockPopup::~SetupForceBlockPopup()
{
}