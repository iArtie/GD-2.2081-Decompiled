#pragma once

#include "cocos2d.h"
#include <string>
#include <unordered_map>

class DS_Dictionary;
class GJGameLevel;

class LocalLevelManager : public cocos2d::CCNode {
public:
    static LocalLevelManager* sharedState();

    void dataLoaded(DS_Dictionary* dict);
    void encodeDataTo(DS_Dictionary* dict);
    void firstLoad();
    cocos2d::CCDictionary* getAllLevelsInDict();
    cocos2d::CCArray* getAllLevelsWithName(std::string name);
    cocos2d::CCArray* getCreatedLevels(int folder);
    cocos2d::CCArray* getCreatedLists(int folder);
    cocos2d::CCDictionary* getLevelsInNameGroups();
    std::string getMainLevelString(int id);
    bool init() override;
    void markLevelsAsUnmodified();
    void moveLevelToTop(GJGameLevel* level);
    void reorderLevels();
    void reorderLists();
    void setup();
    void tryLoadMainLevelString(int id);
    int updateLevelOrder();
    void updateLevelRevision();
    int updateListOrder();

    ~LocalLevelManager();

protected:
    std::string m_fileName;
    cocos2d::CCArray* m_localLevels = nullptr;
    cocos2d::CCArray* m_localLists = nullptr;
    std::unordered_map<int, std::string> m_mainLevels;
};