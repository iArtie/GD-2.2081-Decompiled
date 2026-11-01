#pragma once

class GJGameLevel;

class LevelUpdateDelegate {
public:
    void levelUpdateFailed(int response);
    void levelUpdateFinished(GJGameLevel* level, UpdateResponse response);
};