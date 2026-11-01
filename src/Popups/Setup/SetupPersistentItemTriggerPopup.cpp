#include "Popups/Setup/SetupPersistentItemTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/ItemTriggerGameObject.h"

SetupPersistentItemTriggerPopup* SetupPersistentItemTriggerPopup::create(ItemTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupPersistentItemTriggerPopup* instance = new SetupPersistentItemTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupPersistentItemTriggerPopup::init(ItemTriggerGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 230.0f, 1))
        return false;

    preSetup();

    addTitle("Persistent Item Setup");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Lets you set <cg>ItemID</c> to be <cy>Persistent</c>. This means it does not reset upon player death, but only on "
        "level exit.\n"
        "<cl>TargetAll</c> targets all persistent Item IDs.\n"
        "<cp>Reset</c> resets the target item(s) to 0.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint itemIDPos = center + cocos2d::CCPoint(-40.0f, 40.0f);
    createValueControlAdvanced(
        80, "Item ID", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &itemIDPos,
        Int, 10, 1, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint timerPos = center + cocos2d::CCPoint(80.0f, 40.0f);
    createToggleValueControl(494, "Timer", cocos2d::CCPoint(1.0f, 0.0f), &timerPos, 0, 0, 1.0f);

    cocos2d::CCPoint toggleRowPos(winSize.width * 0.5f, winSize.height * 0.5f - 23.0f);

    cocos2d::CCPoint persistentPos = toggleRowPos + cocos2d::CCPoint(-80.0f, 0.0f);
    createToggleValueControl(491, "Persistent", cocos2d::CCPoint(1.0f, 0.0f), &persistentPos, 0, 0, 1.0f);

    cocos2d::CCPoint targetAllPos = toggleRowPos;
    createToggleValueControl(492, "TargetAll", cocos2d::CCPoint(1.0f, 0.0f), &targetAllPos, 0, 0, 1.0f);

    cocos2d::CCPoint resetPos = toggleRowPos + cocos2d::CCPoint(80.0f, 0.0f);
    createToggleValueControl(493, "Reset", cocos2d::CCPoint(1.0f, 0.0f), &resetPos, 0, 0, 1.0f);

    postSetup();

    return true;
}

SetupPersistentItemTriggerPopup::~SetupPersistentItemTriggerPopup()
{
}