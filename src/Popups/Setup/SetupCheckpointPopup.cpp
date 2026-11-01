#include "Popups/Setup/SetupCheckpointPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/CheckpointGameObject.h"

SetupCheckpointPopup* SetupCheckpointPopup::create(CheckpointGameObject* object, cocos2d::CCArray* objects)
{
    SetupCheckpointPopup* instance = new SetupCheckpointPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupCheckpointPopup::init(CheckpointGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 380.0f, 280.0f, 1))
        return false;

    preSetup();

    addTitle("Setup Checkpoint");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Checkpoints</c> used when the game type is Platformer.\n"
        "<cy>SpawnID</c> lets you spawn a group when the checkpoint is activated.\n"
        "<cl>TargetPos</c> lets you set a respawn location upon death after the checkpoint. Make sure this group contains o"
        "ne object.\n"
        "<cp>RespawnID</c> lets you spawn a group when the player respawns at this checkpoint.",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint spawnIDPos = center + cocos2d::CCPoint(-80.0f, 50.0f);
    createValueControlAdvanced(
        51, "SpawnID:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &spawnIDPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint targetPosPos = center + cocos2d::CCPoint(80.0f, 50.0f);
    createValueControlAdvanced(
        71, "TargetPos:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &targetPosPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint respawnIDPos = center + cocos2d::CCPoint(80.0f, -20.0f);
    createValueControlAdvanced(
        448, "RespawnID:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &respawnIDPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint playerPosPos = center + cocos2d::CCPoint(-80.0f, -20.0f);
    createToggleValueControl(138, "Player Pos", cocos2d::CCPoint(1.0f, 0.0f), &playerPosPos, 1, 0, 0.0f);

    postSetup();

    return true;
}

SetupCheckpointPopup::~SetupCheckpointPopup()
{
}