#pragma once

#include "cocos2d.h"
#include <string>
#include <vector>

class DS_Dictionary;
class GJGameLevel;
class DifficultyIconType;

class GJLevelList : public cocos2d::CCNode {
public:
    void addLevelToList(GJGameLevel* level);
    bool canEncode();
    int completedLevels();
    static GJLevelList* create();
    static GJLevelList* createWithCoder(DS_Dictionary* dict);
    void dataLoaded(DS_Dictionary* dict);
    void duplicateListLevels();
    void encodeWithCoder(DS_Dictionary* dict);
    static std::string frameForListDifficulty(int diff, DifficultyIconType type);
    cocos2d::CCArray* getListLevelsArray(cocos2d::CCArray* levels);
    std::string getUnpackedDescription();
    void handleStatsConflict();
    bool hasMatchingLevels();
    bool init() override;
    int orderForLevel(int id);
    void parseListLevels(const std::string& str);
    void removeLevelFromList(int id);
    void reorderLevel(int levelID, int newPosition);
    void reorderLevelStep(int id, bool up);
    void showListInfo();
    int totalLevels();
    void updateLevelsString();
    
    virtual ~GJLevelList();

private:
    int m_listID;
    std::string m_listName;
    std::string m_listDesc;
    std::string m_creatorName;
    int m_accountID;
    int m_difficulty;
    int m_downloads;
    bool m_uploaded;
    int m_likes;
    int m_listType;
    int m_listVersion;
    bool m_unlisted;
    bool m_friendsOnly;
    int m_listRevision;
    bool m_modified;
    int m_listOrder;
    std::string m_levelsString;
    bool m_favorite;
    bool m_featured;
    int m_original;
    int m_uploadDate;
    int m_updateDate;
    int m_diamonds;
    int m_levelsToClaim;
    bool m_onlineLevelsLoaded;
    cocos2d::CCDictionary* m_levelsDict;
    int m_M_ID;
    std::vector<int> m_levels;
    int m_folder;
    
    static int s_nextId;
};