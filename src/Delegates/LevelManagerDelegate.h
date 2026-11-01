#pragma once

#include "Platform/cocos2d.h"
#include <string>

class LevelManagerDelegate {
public:
    void loadLevelsFailed(const char *key);
    void loadLevelsFinished(cocos2d::CCArray *levels, const char *key);
    void setupPageInfo(std::string info, const char *key);
};
