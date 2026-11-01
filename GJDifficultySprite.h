// GJDifficultySprite.h
#pragma once

#include "cocos2d.h"
#include <string>

class GJGameLevel;

enum class GJDifficultyName {
    Short = 0,
    Long = 1
};

enum class GJFeatureState {
    None = 0,
    Featured = 1,
    Epic = 2,
    Legendary = 3,
    Mythic = 4
};

class GJDifficultySprite : public cocos2d::CCSprite {
public:
    static GJDifficultySprite* create(int difficulty, GJDifficultyName name);
    static std::string getDifficultyFrame(int difficulty, GJDifficultyName name);
    bool init(int difficulty, GJDifficultyName name);
    void updateDifficultyFrame(int difficulty, GJDifficultyName name);
    void updateFeatureState(GJFeatureState state);
    void updateFeatureStateFromLevel(GJGameLevel* level);
    virtual ~GJDifficultySprite();

private:
    GJFeatureState m_featureState;
};