#include "Popups/Setup/SetupAreaTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Popups/Setup/SetupAreaMoveTriggerPopup.h"
#include "Objects/EnterEffectObject.h"

SetupAreaTriggerPopup* SetupAreaTriggerPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects)
{
    SetupAreaTriggerPopup* instance = new SetupAreaTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupAreaTriggerPopup::init(EnterEffectObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 420.0f, 280.0f, 1))
        return false;

    determineStartValues();

    addTitle("Area Trigger");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Alters objects using <cg>Target Group ID</c> within an area around <cy>Center Group ID</c>. The area can be specif"
        "ied by the arrow buttons and <cl>Length</c> (for size).\n"
        "You can specify how much the objects in the area are altered depending on how close they are to the center.\n"
        "Check the other pages for more settings.",
        1.0f);

    createMultiTriggerItemsDefaultHorizontal();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint basePoint(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f);

    cocos2d::CCPoint offset1 = basePoint + cocos2d::CCPoint(-75.0f, 0.0f);
    createValueControlAdvanced(
        222,
        "Length",
        cocos2d::CCPoint(0.65f, 0.0f),
        100.0f,
        &offset1,
        (InputValueType)0,
        2,
        6,
        0.0f,
        0.0f,
        0,
        0,
        GoldLabel,
        1,
        2);

    cocos2d::CCPoint offset2 = basePoint + cocos2d::CCPoint(75.0f, 0.0f);
    createValueControlAdvanced(
        223,
        "+-",
        cocos2d::CCPoint(0.65f, 0.0f),
        100.0f,
        &offset2,
        (InputValueType)0,
        2,
        6,
        0.0f,
        0.0f,
        0,
        0,
        GoldLabel,
        1,
        2);

    cocos2d::CCPoint offset3 = basePoint + cocos2d::CCPoint(-75.0f, -25.0f);
    createValueControlAdvanced(
        220,
        "Offset",
        cocos2d::CCPoint(0.65f, -100.0f),
        100.0f,
        &offset3,
        (InputValueType)0,
        2,
        6,
        0.0f,
        0.0f,
        0,
        0,
        GoldLabel,
        1,
        2);

    cocos2d::CCPoint offset4 = basePoint + cocos2d::CCPoint(75.0f, -25.0f);
    createValueControlAdvanced(
        221,
        "+-",
        cocos2d::CCPoint(0.65f, -100.0f),
        100.0f,
        &offset4,
        (InputValueType)0,
        2,
        6,
        0.0f,
        0.0f,
        0,
        0,
        GoldLabel,
        1,
        2);

    SetupAreaMoveTriggerPopup::addAreaDefaultControls(0);

    updateDefaultTriggerValues();

    createPageButtons(m_width * 0.5f + 15.0f, 1);
    goToPage(0, false);

    m_disableTextDelegate = false;

    return true;
}

SetupAreaTriggerPopup::~SetupAreaTriggerPopup()
{
}