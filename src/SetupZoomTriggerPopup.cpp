#include "SetupZoomTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupZoomTriggerPopup* SetupZoomTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupZoomTriggerPopup* instance = new SetupZoomTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupZoomTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 360.0f, 220.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Setup Zoom");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Zooms</c> the camera a given amount.\n"
        "<cy>Time</c> is how fast the zoom happens.\n"
        "<cl>Easing</c> changes the method used.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint zoomPos = center + cocos2d::CCPoint(0.0f, 55.0f);
    createValueControlAdvanced(
        371, "Zoom:", cocos2d::CCPoint(0.7f, 0.4f), 3.0f, &zoomPos,
        (InputValueType)0, 1, 4, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 3);

    cocos2d::CCPoint timePos = center + cocos2d::CCPoint(-80.0f, 0.0f);
    createValueControlAdvanced(
        10, "Time:", cocos2d::CCPoint(0.7f, 0.0f), 10.0f, &timePos,
        (InputValueType)0, 1, 4, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint easingPos = center + cocos2d::CCPoint(80.0f, 0.0f);
    createEasingControls(cocos2d::CCPoint(0.7f, 0.0f), 0.0f, &easingPos, 0);

    postSetup();

    m_disableTextDelegate = false;

    return true;
}

SetupZoomTriggerPopup::~SetupZoomTriggerPopup()
{
}