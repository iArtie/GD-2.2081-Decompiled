#include "Popups/Setup/SetupBGSpeedTrigger.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"

SetupBGSpeedTrigger* SetupBGSpeedTrigger::create(EffectGameObject* object, cocos2d::CCArray* objects, int id)
{
    SetupBGSpeedTrigger* instance = new SetupBGSpeedTrigger();
    if (instance->init(object, objects, id))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupBGSpeedTrigger::init(EffectGameObject* object, cocos2d::CCArray* objects, int id)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 240.0f, 1))
        return false;

    preSetup();

    const char* title = (id == 3612) ? "Change MG Speed" : "Change BG Speed";
    addTitle(title);
    addCloseButton("OK");

    if (id == 3612)
    {
        addHelpButton(
            "Help",
            "Lets you change the speed of the <cg>middle ground</c> during gameplay movement. Default X: 0.3, Y: 0.5",
            1.0f);
    }
    else
    {
        addHelpButton(
            "Help",
            "Lets you change the speed of the <cg>background</c> during gameplay movement. Default X: 0.1, Y: 0.1",
            1.0f);
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint modXPos = center + cocos2d::CCPoint(0.0f, 60.0f);
    createValueControlAdvanced(
        143, "Mod X:", cocos2d::CCPoint(0.8f, 0.0f), 1.0f, &modXPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint modYPos = center + cocos2d::CCPoint(0.0f, -10.0f);
    createValueControlAdvanced(
        144, "Mod Y:", cocos2d::CCPoint(0.8f, 0.0f), 1.0f, &modYPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    createMultiTriggerItemsDefault();
    postSetup();

    return true;
}

SetupBGSpeedTrigger::~SetupBGSpeedTrigger()
{
}