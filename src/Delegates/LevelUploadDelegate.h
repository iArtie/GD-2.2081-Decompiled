#pragma once

class GJGameLevel;

class LevelUploadDelegate {
public:
    void levelUploadFailed(GJGameLevel* level);
    void levelUploadFinished(GJGameLevel* level);
};