#include "SetupKeyframeAnimPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupKeyframeAnimPopup* SetupKeyframeAnimPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupKeyframeAnimPopup* instance = new SetupKeyframeAnimPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupKeyframeAnimPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 340.0f, 280.0f, 1))
        return false;

    preSetup();

    addTitle("Setup Keyframe Anim");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Sets up a <cg>keyframe animation</c> using <cy>Animation Group ID</c>.\n"
        "Use <cl>Target ID</c> to specify a different target than the one set in the Keyframe trigger.\n"
        "The animation itself is made in the Keyframe trigger.\n"
        "By default, objects choose their own center for scaling and rotation, but you can set one using <cp>Parent ID</c>."
        "\n"
        "On page 2, you can put modifiers for the keyframe sequence.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint base(winSize.width * 0.5f, winSize.height * 0.5f + 60.0f);

    cocos2d::CCPoint animGroupPos = base;
    createValueControlAdvanced(
        76, "Animation Group ID", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &animGroupPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint targetIDPos = base + cocos2d::CCPoint(-85.0f, -70.0f);
    createValueControlAdvanced(
        51, "Target ID", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &targetIDPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint parentIDPos = base + cocos2d::CCPoint(85.0f, -70.0f);
    createValueControlAdvanced(
        71, "Parent ID", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &parentIDPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint pageBase(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f);

    cocos2d::CCPoint timeModPos = pageBase;
    createValueControlAdvanced(
        520, "Time Mod:", cocos2d::CCPoint(0.7f, 0.0f), 1.0f, &timeModPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 1, GoldLabel, 1, 2);

    cocos2d::CCPoint posXModPos = pageBase + cocos2d::CCPoint(0.0f, -30.0f);
    createValueControlAdvanced(
        521, "Position X Mod:", cocos2d::CCPoint(0.7f, -2.0f), 2.0f, &posXModPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 1, GoldLabel, 1, 2);

    cocos2d::CCPoint posYModPos = pageBase + cocos2d::CCPoint(0.0f, -60.0f);
    createValueControlAdvanced(
        545, "Position Y Mod:", cocos2d::CCPoint(0.7f, -2.0f), 2.0f, &posYModPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 1, GoldLabel, 1, 2);

    cocos2d::CCPoint rotModPos = pageBase + cocos2d::CCPoint(0.0f, -90.0f);
    createValueControlAdvanced(
        522, "Rotation mod:", cocos2d::CCPoint(0.7f, -2.0f), 2.0f, &rotModPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 1, GoldLabel, 1, 2);

    cocos2d::CCPoint scaleXModPos = pageBase + cocos2d::CCPoint(0.0f, -120.0f);
    createValueControlAdvanced(
        523, "Scale X Mod:", cocos2d::CCPoint(0.7f, -2.0f), 2.0f, &scaleXModPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 1, GoldLabel, 1, 2);

    cocos2d::CCPoint scaleYModPos = pageBase + cocos2d::CCPoint(0.0f, -150.0f);
    createValueControlAdvanced(
        546, "Scale Y Mod:", cocos2d::CCPoint(0.7f, -2.0f), 2.0f, &scaleYModPos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 1, GoldLabel, 1, 2);

    createPageButtons(m_width * 0.5f + 15.0f, 1);
    goToPage(0, false);

    postSetup();

    return true;
}

SetupKeyframeAnimPopup::~SetupKeyframeAnimPopup()
{
}