#include "Popups/Setup/SetupPickupTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"
#include "Managers/GameManager.h"
#include "Layers/LevelEditorLayer.h"

SetupPickupTriggerPopup* SetupPickupTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupPickupTriggerPopup* instance = new SetupPickupTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupPickupTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 250.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Pickup Trigger");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Changes the value of an <cg>Item ID</c> by adding <cy>Count</c> to the previous value.\n"
        "Use <cl>Multiply or Divide</c> to modify in another way than addition.\n"
        "<cp>Override</c> specifies the exact value to change to instead of modifying the previous value.",
        1.0f);

    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint itemIDPos = center + cocos2d::CCPoint(-50.0f, 50.0f);
    createValueControlAdvanced(
        80, "ItemID", cocos2d::CCPoint(0.8f, 0.0f), 100.0f, &itemIDPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint plusPos = center + cocos2d::CCPoint(-5.0f, 80.0f);
    createPlusButton(80, cocos2d::CCPoint(0.8f, 0.0f), 0.0f, "", &plusPos, 0);

    int multiplyMode = (int)getValue(88);

    cocos2d::CCPoint multiplyPos = center + cocos2d::CCPoint(50.0f, 70.0f);
    createCustomToggleValueControl(1, multiplyMode == 1, 1, "Multiply",
        cocos2d::CCPoint(1.0f, 0.0f), &multiplyPos, 0, 0);

    cocos2d::CCPoint dividePos = center + cocos2d::CCPoint(50.0f, 30.0f);
    createCustomToggleValueControl(2, multiplyMode == 2, 1, "Divide",
        cocos2d::CCPoint(1.0f, 0.0f), &dividePos, 0, 0);

    cocos2d::CCPoint countPos = center + cocos2d::CCPoint(-50.0f, -20.0f);
    createValueControlAdvanced(
        77, "Count", cocos2d::CCPoint(0.8f, 0.0f), 100.0f, &countPos,
        Float, 2, 6, 0.0f, 0.0f, 1, 0, ChatLabel, 0, 2);

    cocos2d::CCPoint overridePos = center + cocos2d::CCPoint(50.0f, -20.0f);
    createToggleValueControl(139, "Override", cocos2d::CCPoint(1.0f, 0.0f), &overridePos, 0, 0, 0.0f);

    cocos2d::CCPoint modifierPos = center + cocos2d::CCPoint(0.0f, -20.0f);
    createValueControlAdvanced(
        449, "Modifier:", cocos2d::CCPoint(0.8f, 0.0f), 10.0f, &modifierPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, (GJInputStyle)2, 0, 3);

    createMultiTriggerItemsDefault();

    updateState();

    postSetup();

    return true;
}

void SetupPickupTriggerPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    bool toggle1 = false;
    bool toggle2 = false;

    if (tag == (int)getValue(88))
    {
        tag = 0;
    }
    else
    {
        toggle1 = (tag == 1);
        toggle2 = (tag == 2);
    }

    updateCustomToggleTrigger(1, toggle1);
    updateCustomToggleTrigger(2, toggle2);
    updateValue(88, (float)tag);
    updateState();
}

void SetupPickupTriggerPopup::onPlusButton(cocos2d::CCObject* sender)
{
    std::unordered_set<int> usedIDs;
    int nextID = GameManager::sharedState()->m_levelEditorLayer->getNextFreeItemID(&usedIDs);
    updateValue(80, (float)nextID);
}

void SetupPickupTriggerPopup::updateState()
{
    float value = getValue(88);
    toggleGroup(1, value == 0.0f);
    toggleGroup(2, value != 0.0f);
}

SetupPickupTriggerPopup::~SetupPickupTriggerPopup()
{
}