#include "MultiTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameToolbox.h"

MultiTriggerPopup* MultiTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    MultiTriggerPopup* ret = new MultiTriggerPopup();
    if (ret->init(object, objects)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool MultiTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 200.0f, 120.0f, 1))
        return false;

    preSetup();

    addTitle("Setup");
    addCloseButton("OK");

    addHelpButton(
        "Help",
        "<cy>Allow Multi</c> makes it so the object can be activated several times.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint pos(winSize.width * 0.5f - 30.0f, winSize.height * 0.5f + 10.0f);

    createToggleValueControl(87, "Allow Multi", ccp(1.0f, 0.5f), pos, false, 0, 1.0f);

    postSetup();

    return true;
}

MultiTriggerPopup::~MultiTriggerPopup()
{
}