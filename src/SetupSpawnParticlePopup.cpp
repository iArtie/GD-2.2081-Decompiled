#include "SetupSpawnParticlePopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupSpawnParticlePopup* SetupSpawnParticlePopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupSpawnParticlePopup* instance = new SetupSpawnParticlePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupSpawnParticlePopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 380.0f, 280.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Spawn Particles");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Spawns particles</c> given by <cy>Particle Group</c> at a target location given by <cl>Position Group</c>.\n"
        "It can be more than one particle object at once.\n"
        "<cp>Offset X and Y</c> offsets the target position.\n"
        "<cr>OffVarX and Y</c> set an area the particles can randomly spawn in.\n"
        "<co>Match Rot</c> makes the rotation of several particles match.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCPoint base(winSize.width * 0.5f, winSize.height * 0.5f + 80.0f);

    cocos2d::CCPoint particlePos = base + cocos2d::CCPoint(-95.0f, 0.0f);
    createValueControlAdvanced(
        51, "Particle Group", cocos2d::CCPoint(0.6f, 0.0f), 100.0f, &particlePos,
        Float, 3, 6, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 2);

    cocos2d::CCPoint positionPos = base + cocos2d::CCPoint(65.0f, 0.0f);
    createValueControlAdvanced(
        71, "Position Group", cocos2d::CCPoint(0.6f, 0.0f), 100.0f, &positionPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 2);

    cocos2d::CCPoint offsetXPos = base + cocos2d::CCPoint(-80.0f, -33.0f);
    createValueControlAdvanced(
        547, "Offset X:", cocos2d::CCPoint(0.6f, -100.0f), 100.0f, &offsetXPos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 0);

    cocos2d::CCPoint offsetYPos = base + cocos2d::CCPoint(80.0f, -33.0f);
    createValueControlAdvanced(
        548, "Y:", cocos2d::CCPoint(0.6f, -100.0f), 100.0f, &offsetYPos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 0);

    cocos2d::CCPoint offVarXPos = base + cocos2d::CCPoint(-80.0f, -66.0f);
    createValueControlAdvanced(
        549, "OffVar X:", cocos2d::CCPoint(0.6f, 0.0f), 100.0f, &offVarXPos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 0);

    cocos2d::CCPoint offVarYPos = base + cocos2d::CCPoint(80.0f, -66.0f);
    createValueControlAdvanced(
        550, "Y:", cocos2d::CCPoint(0.6f, 0.0f), 100.0f, &offVarYPos,
        (InputValueType)0, 2, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 0);

    cocos2d::CCPoint rotationPos = base + cocos2d::CCPoint(-80.0f, -99.0f);
    createValueControlAdvanced(
        552, "Rotation:", cocos2d::CCPoint(0.6f, -180.0f), 180.0f, &rotationPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 0);

    cocos2d::CCPoint rotationVarPos = base + cocos2d::CCPoint(80.0f, -99.0f);
    createValueControlAdvanced(
        553, "+-", cocos2d::CCPoint(0.6f, 0.0f), 180.0f, &rotationVarPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 0);

    cocos2d::CCPoint scalePos = base + cocos2d::CCPoint(-80.0f, -132.0f);
    createValueControlAdvanced(
        554, "Scale:", cocos2d::CCPoint(0.6f, -2.0f), 2.0f, &scalePos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    cocos2d::CCPoint scaleVarPos = base + cocos2d::CCPoint(80.0f, -132.0f);
    createValueControlAdvanced(
        555, "+-", cocos2d::CCPoint(0.6f, 0.0f), 1.0f, &scaleVarPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    cocos2d::CCPoint matchRotPos(
        m_width * 0.5f + winSize.width * 0.5f - 72.0f,
        50.0f - (m_height * 0.5f - winSize.height * 0.5f));
    createToggleValueControl(551, "Match\nRot", cocos2d::CCPoint(1.0f, 0.0f), &matchRotPos, 0, 0, 0.0f);

    postSetup();

    return true;
}

SetupSpawnParticlePopup::~SetupSpawnParticlePopup()
{
}