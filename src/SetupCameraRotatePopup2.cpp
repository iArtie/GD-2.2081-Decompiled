#include "SetupCameraRotatePopup2.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupCameraRotatePopup2* SetupCameraRotatePopup2::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupCameraRotatePopup2* instance = new SetupCameraRotatePopup2();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupCameraRotatePopup2::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 400.0f, 260.0f, 1))
        return false;

    preSetup();

    addTitle("Setup Camera Rotation");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Rotates</c> the camera to the target value.\n"
        "<cy>Move Time</c> is how fast the rotate happens.\n"
        "<cl>Easing</c> changes the method used.\n"
        "<cg>Add</c> the rotation to the current camera rotation.\n"
        "<cp>Snap360</c> converts rotation to closest 360.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint degreesPos = center + cocos2d::CCPoint(-100.0f, 60.0f);
    createValueControlAdvanced(
        68, "Degrees:", cocos2d::CCPoint(0.8f, -360.0f), 360.0f, &degreesPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint addPos = center + cocos2d::CCPoint(60.0f, 70.0f);
    createToggleValueControl(70, "Add", cocos2d::CCPoint(1.0f, 0.0f), &addPos, 0, 0, 0.0f);

    cocos2d::CCPoint snapPos = center + cocos2d::CCPoint(60.0f, 30.0f);
    createToggleValueControl(394, "Snap360", cocos2d::CCPoint(1.0f, 0.0f), &snapPos, 0, 0, 0.0f);

    cocos2d::CCPoint moveTimePos = center + cocos2d::CCPoint(-100.0f, -10.0f);
    createValueControlAdvanced(
        10, "Move Time:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &moveTimePos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint easingPos = center + cocos2d::CCPoint(95.0f, -10.0f);
    createEasingControls(cocos2d::CCPoint(0.8f, 0.0f), 0.0f, &easingPos, 0);

    postSetup();

    return true;
}

SetupCameraRotatePopup2::~SetupCameraRotatePopup2()
{
}