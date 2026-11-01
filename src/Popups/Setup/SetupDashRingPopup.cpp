#include "Popups/Setup/SetupDashRingPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/DashRingObject.h"

SetupDashRingPopup* SetupDashRingPopup::create(DashRingObject* object, cocos2d::CCArray* objects)
{
    SetupDashRingPopup* instance = new SetupDashRingPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupDashRingPopup::init(DashRingObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 320.0f, 240.0f, 1))
        return false;

    preSetup();

    addTitle("Dash Orb");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Speed</c> changes the speed of the dash.\n"
        "<cy>End Boost</c> is the force applied when the dash stops.\n"
        "<cl>Max Duration</c> will end the dash automatically after this time.\n"
        "<co>Allow Collide</c> makes the dash not end on collision.\n"
        "<cp>Stop Slide</c> will limit the end momentum.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint base(winSize.width * 0.5f, winSize.height * 0.5f + 70.0f);

    cocos2d::CCPoint speedPos = base + cocos2d::CCPoint(0.0f, 0.0f);
    createValueControlAdvanced(
        586, "Speed", cocos2d::CCPoint(0.8f, 0.0f), 2.0f, &speedPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    cocos2d::CCPoint endBoostPos = base + cocos2d::CCPoint(0.0f, -40.0f);
    createValueControlAdvanced(
        588, "End Boost", cocos2d::CCPoint(0.8f, 0.0f), 2.0f, &endBoostPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    cocos2d::CCPoint maxDurPos = base + cocos2d::CCPoint(0.0f, -80.0f);
    createValueControlAdvanced(
        590, "Max Duration", cocos2d::CCPoint(0.8f, 0.0f), 2.0f, &maxDurPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    cocos2d::CCPoint allowCollidePos = base + cocos2d::CCPoint(-80.0f, -120.0f);
    createToggleValueControl(587, "Allow\nCollide", cocos2d::CCPoint(1.0f, 0.0f), &allowCollidePos, 0, 0, 0.0f);

    cocos2d::CCPoint stopSlidePos = base + cocos2d::CCPoint(20.0f, -120.0f);
    createToggleValueControl(589, "Stop\nSlide", cocos2d::CCPoint(1.0f, 0.0f), &stopSlidePos, 0, 0, 0.0f);

    cocos2d::CCPoint noMultiPos(
        20.0f - (m_width * 0.5f - winSize.width * 0.5f),
        25.0f - (m_height * 0.5f - winSize.height * 0.5f));
    createToggleValueControl(444, "NO Multi\nActivate", cocos2d::CCPoint(1.0f, 0.0f), &noMultiPos, 0, 0, 0.0f);

    postSetup();

    return true;
}

SetupDashRingPopup::~SetupDashRingPopup()
{
}