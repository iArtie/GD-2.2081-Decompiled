// GJGameLevel.h
#pragma once

#include "Platform/cocos2d.h"
#include <string>
#include <vector>

class DS_Dictionary;

enum class GJLevelType {
    Editor = 0,
    Main = 1,
    // otros
};

enum class DemonDifficultyType {
    // valores
};

struct GJGameValue {
    int value;
    int seed;
    int random;
};

class GJGameLevel : public cocos2d::CCObject {
public:
    static GJGameLevel* create();
    static GJGameLevel* createWithCoder(DS_Dictionary* dict);
    bool init();
    virtual ~GJGameLevel();

    bool areCoinsVerified();
    bool canEncode();
    void copyLevelInfo();
    void dataLoaded(DS_Dictionary* dict);
    static int demonIconForDifficulty(DemonDifficultyType type);
    void encodeWithCoder(DS_Dictionary* dict);
    std::string generateSettingsString();
    std::string getAudioFileName();
    int getAverageDifficulty();
    const char* getCoinKey(int coinNumber);
    int getLastBuildPageForTab(int tab);
    static int getLengthKey(int length, bool platformer);
    GJGameLevel* getListSnapshot();
    int getNormalPercent();
    std::string getSongName();
    std::string getUnpackedLevelDescription();
    void handleStatsConflict();
    bool isPlatformer();
    static std::string lengthKeyToString(int key);
    void levelWasAltered();
    void levelWasSubmitted();
    void parseSettingsString(std::string str);
    void scoreStringToVector(std::string* str, std::vector<int>* vec);
    std::string scoreVectorToString(std::vector<int>* vec, int type);
    void setLastBuildPageForTab(int tab, int page);
    void setNormalPercent(int percent);
    bool shouldCheatReset();
    void storeNewLocalScore(int value, int type);
    void unverifyCoins();

    std::string m_levelName;
    std::string m_levelDesc;
    std::string m_levelString;
    std::string m_personalBests;
    GJLevelType m_levelType;
    int m_levelVersion;
    GJGameValue m_attempts;
    int m_M_ID;
    GJGameValue m_jumps;
    GJGameValue m_clicks;
    GJGameValue m_attemptTime;
    int m_chk;
    int m_coins;
    GJGameValue m_stars;
    GJGameValue m_demon;
    bool m_dontSave;
    bool m_levelNotDownloaded;
    cocos2d::CCDictionary* m_lastBuildSave;
    bool m_isUploaded;
    int m_ratings;
    int m_ratingsSum;
    GJGameValue m_dailyID;
    GJGameValue m_levelID;
    bool m_gauntletLevel;
    int m_songID;
    int m_audioTrack;
    GJGameValue m_normalPercent;
    bool m_isChkValid;
    int m_levelLength;
    GJGameValue m_orbCompletion;
    GJGameValue m_newNormalPercent2;
    int m_practicePercent;
    int m_demonDifficulty;
    bool m_isCompletionLegitimate;
    bool m_gauntletLevel2;
    bool m_levelFavorited;
    int m_levelFolder;
    int m_bestTime;
    int m_bestPoints;
    int m_ticksTime;
    int m_clicksTime;
    int m_coinsTime;
    bool m_savedTime;
    std::string m_inputsTime;
    int m_ticksPoints;
    int m_clicksPoints;
    int m_coinsPoints;
    bool m_savedPoints;
    std::string m_inputsPoints;
    int m_platformerSeed;
    std::string m_localBestTimes;
    std::string m_localBestPoints;
    std::string m_tempName;
    std::string m_creatorName;
    int m_timestamp;
    std::string m_lockedEditorLayers;
    std::string m_savedCameraPositions;
    int m_cbsOverride;
    cocos2d::CCPoint m_previewLock;
    GJGameValue m_userID;
    GJGameValue m_accountID;
    int m_difficulty;
    int m_downloads;
    bool m_isEditable;
    GJGameValue m_isVerified;
    int m_likes;
    int m_dislikes;
    int m_gameVersion;
    bool m_showedSongWarning;
    bool m_highObjectsEnabled;
    bool m_unlimitedObjectsEnabled;
    int m_workingTime;
    int m_workingTime2;
    bool m_lowDetailMode;
    bool m_lowDetailModeToggled;
    bool m_disableShakeToggled;
    int m_levelIndex;
    GJGameValue m_coinsVerified;
    int m_featured;
    int m_isEpic;
    bool m_autoLevel;
    bool m_unlisted;
    bool m_friendsOnly;
    GJGameValue m_password;
    GJGameValue m_originalLevel;
    bool m_twoPlayerMode;
    int m_levelRev;
    bool m_hasBeenModified;
    GJGameValue m_objectCount;
    std::string m_capacityString;
    std::string m_songIDs;
    std::string m_sfxIDs;
    int m_songSize;
    int m_starsRequested;
    bool m_isUnlocked;
    int m_requiredCoins;
    cocos2d::CCPoint m_lastCameraPos;
    float m_lastEditorZoom;
    int m_lastBuildPage;
    int m_lastBuildTab;
    int m_lastBuildGroupID;
    GJGameValue m_firstCoinVerified;
    GJGameValue m_secondCoinVerified;
    GJGameValue m_thirdCoinVerified;
    std::string m_recordString;
    bool m_isVerifiedRaw;
};