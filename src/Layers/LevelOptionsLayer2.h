#pragma once

#include "Layers/LevelOptionsLayer.h"

class LevelSettingsObject;

class LevelOptionsLayer2 : public LevelOptionsLayer {
public:
    static LevelOptionsLayer2* create(LevelSettingsObject* object);
    bool init(LevelSettingsObject* object);
    void setupOptions() override;
    ~LevelOptionsLayer2();
};