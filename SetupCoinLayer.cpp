#include "SetupCoinLayer.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupCoinLayer* SetupCoinLayer::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupCoinLayer* instance = new SetupCoinLayer();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupCoinLayer::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 220.0f, 1))
        return false;

    preSetup();

    addTitle("Setup Coin");
    addCloseButton("OK");
    addHelpButton("Help", "Target ID will be spawned triggered when collected.", 1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint targetIDPos = center + cocos2d::CCPoint(0.0f, 30.0f);
    createValueControlAdvanced(
        51, "Target ID:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &targetIDPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    if (object && object->m_objectID == 142)
    {
        cocos2d::CCPoint uniqueIDPos = center + cocos2d::CCPoint(0.0f, -30.0f);
        createValueControlAdvanced(
            12, "Unique ID:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &uniqueIDPos,
            Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);
    }

    postSetup();

    return true;
}

SetupCoinLayer::~SetupCoinLayer()
{
}