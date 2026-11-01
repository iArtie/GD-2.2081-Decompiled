#pragma once

#include "EffectGameObject.h"
#include <string>

class GJBaseGameLayer;

class CheckpointGameObject : public EffectGameObject {
public:
    static CheckpointGameObject* create();

    bool init() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    void resetObject() override;
    void restoreObject() override;
    void setupCustomSprites(std::string frameName) override;
    void triggerActivated(float xPosition) override;
    void updateSyncedAnimation(float totalTime, int frameIndex) override;

    void resetCheckpoint();
    void updateCheckpointSpriteVisibility();

    ~CheckpointGameObject();

private:
    bool m_checkpointActivated;
    bool m_isColorSpriteBlack;
    int m_targetGroupID;
    int m_centerGroupID;
    int m_respawnID;
    bool m_targetPlayer1;
};