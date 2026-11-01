#pragma once

class LevelDeleteDelegate {
public:
    virtual ~LevelDeleteDelegate() = default;
    virtual void levelDeleteFailed(int id);
    virtual void levelDeleteFinished(int id);
};