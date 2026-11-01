#pragma once

class LevelListDeleteDelegate {
public:
    void levelListDeleteFailed(int id);
    void levelListDeleteFinished(int id);
};