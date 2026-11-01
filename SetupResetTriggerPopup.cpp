#include "SetupResetTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupResetTriggerPopup* SetupResetTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupResetTriggerPopup* instance = new SetupResetTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupResetTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 220.0f, 1))
        return false;

    preSetup();

    addTitle("Reset Trigger");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Resets</c> certain objects, such as collectables and checkpoints, given by <cy>Group ID</c>. These will then r"
        "eappear and can be collected again.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    createValueControlAdvanced(
        51, "Group ID:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &center,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    postSetup();

    return true;
}

SetupResetTriggerPopup::~SetupResetTriggerPopup()
{
}