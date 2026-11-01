#pragma once

#include "cocos2d.h"
#include <string>

class GJMapPack : public cocos2d::CCObject {
public:
    int completedMaps();
    static GJMapPack* create();
    bool hasCompletedMapPack();
    bool init();
    void parsePackColors(const std::string& textColorStr, const std::string& barColorStr);
    void parsePackLevels(const std::string& levelsStr);
    int totalMaps();
    virtual ~GJMapPack();

protected:
    std::string m_packName;
    cocos2d::CCArray* m_levels;
    int m_stars;
    int m_coins;
    int m_MId;
    int m_packID;
    cocos2d::ccColor3B m_textColour;
    cocos2d::ccColor3B m_barColour;
    bool m_isGauntlet;
    GJDifficulty m_difficulty;     
    std::string m_levelStrings;    
    
    static int s_nextId;
};