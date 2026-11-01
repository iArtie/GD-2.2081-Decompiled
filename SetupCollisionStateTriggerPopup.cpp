#include "SetupCollisionStateTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupCollisionStateTriggerPopup* SetupCollisionStateTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupCollisionStateTriggerPopup* instance = new SetupCollisionStateTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupCollisionStateTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 340.0f, 200.0f, 1))
        return false;

    preSetup();

    addTitle("Collision State");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Activates a group when the player collides with the hitbox.\n"
        "<cg>State On</c> is activated upon entering.\n"
        "<cy>State Off</c> is activated upon exiting.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f + 10.0f;

    cocos2d::CCPoint stateOnPos(centerX - 70.0f, centerY);
    createValueControlAdvanced(
        51, "State On", cocos2d::CCPoint(0.7f, 0.0f), 100.0f, &stateOnPos,
        (InputValueType)1, 3, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 2, 0);

    cocos2d::CCPoint stateOffPos(centerX + 70.0f, centerY);
    createValueControlAdvanced(
        71, "State Off", cocos2d::CCPoint(0.7f, 0.0f), 100.0f, &stateOffPos,
        (InputValueType)1, 3, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 2, 0);

    postSetup();

    return true;
}

SetupCollisionStateTriggerPopup::~SetupCollisionStateTriggerPopup()
{
}