#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>

class GJGameLevel;
class GJMapPack;
class GJLevelList;
class GJChallengeItem;
class GJRewardItem;
class GJStoreItem;
class GJUnlockableItem;
class DS_Dictionary;

class GameStatsManager : public cocos2d::CCNode {
public:
    static GameStatsManager* sharedState();

    bool init() override;
    ~GameStatsManager() override;

    void firstSetup();
    void dataLoaded(DS_Dictionary* dict);
    void encodeDataTo(DS_Dictionary* dict);
    void postLoadGameStats();
    void preSaveGameStats();

    void createStoreItems();
    void createSecretChestItems();
    void createSecretChestRewards();
    void createSpecialChestItems();
    void generateItemUnlockableData();
    void setupIconCredits();

    void awardCurrencyForLevel(GJGameLevel* level);
    void awardDiamondsForLevel(GJGameLevel* level);
    void awardOldSpecialStats();
    bool awardSecretKey();
    bool shouldAwardSecretKey();

    void completedLevel(GJGameLevel* level);
    void completedDemonLevel(GJGameLevel* level);
    void completedStarLevel(GJGameLevel* level);
    void completedMapPack(GJMapPack* pack);
    void completedChallenge(GJChallengeItem* item);
    GJRewardItem* completedDailyLevel(GJGameLevel* level);

    void markLevelAsCompletedAndClaimed(GJGameLevel* level);
    void uncompleteLevel(GJGameLevel* level);

    void checkAchievement(const char* statKey);
    void checkCoinAchievement(GJGameLevel* level);
    void checkCoinsForLevel(GJGameLevel* level);

    int getStat(const char* key);
    int getStatFromKey(StatKey key);
    void setStat(const char* key, int value);
    void setStatIfHigher(const char* key, int value);
    void incrementStat(const char* key);
    void incrementStat(const char* key, int amount);
    void incrementChallenge(GJChallengeType type, int amount);
    void incrementActivePath(int amount);

    void resetSpecialStatAchievements();
    void recountSpecialStats();
    void recountUserCoins(bool force);
    void verifyUserCoins();
    void verifyPathAchievements();

    int getBaseCurrency(int stars, bool mainLevel, int levelID);
    int getBaseCurrencyForLevel(GJGameLevel* level);
    int getBaseDiamonds(int stars);
    int getBonusDiamonds(int stars);
    int getAwardedCurrencyForLevel(GJGameLevel* level);
    int getAwardedDiamondsForLevel(GJGameLevel* level);
    int getTotalCollectedCurrency();
    int getTotalCollectedDiamonds();
    int getCollectedCoinsForLevel(GJGameLevel* level);

    std::string getLevelKey(GJGameLevel* level);
    std::string getCurrencyKey(GJGameLevel* level);
    std::string getDailyLevelKey(int dailyID);
    const char* getDemonLevelKey(GJGameLevel* level);
    const char* getStarLevelKey(GJGameLevel* level);
    const char* getMapPackKey(int id);
    int starsForMapPack(int id);
    void setStarsForMapPack(int id, int stars);
    cocos2d::CCArray* getCompletedMapPacks();

    bool hasCompletedLevel(GJGameLevel* level);
    bool hasCompletedMainLevel(int levelID);
    bool hasCompletedOnlineLevel(int id);
    bool hasCompletedGauntletLevel(int id);
    bool hasCompletedStarLevel(GJGameLevel* level);
    bool hasCompletedDemonLevel(GJGameLevel* level);
    bool hasCompletedMapPack(int id);
    bool hasCompletedDailyLevel(int dailyID);

    bool hasUserCoin(const char* key);
    bool hasPendingUserCoin(const char* key);
    bool hasSecretCoin(const char* key);
    void storeUserCoin(const char* key);
    void storePendingUserCoin(const char* key);
    void storeSecretCoin(const char* key);
    void resetUserCoins();
    void logCoins();
    bool isSecretCoin(std::string key);
    bool isSecretCoinValid(std::string key);
    const char* getSecretCoinKey(const char* key);

    void registerRewardsFromItem(GJRewardItem* item);
    void preProcessReward(GJRewardItem* item);
    void collectReward(GJRewardType type, GJRewardItem* item);
    void collectVideoReward(int orbs);
    bool hasRewardBeenCollected(GJRewardType type, int id);

    std::string getChallengeKey(GJChallengeItem* chal);
    GJChallengeItem* getChallenge(int id);
    GJChallengeItem* getQueuedChallenge(int id);
    GJChallengeItem* getSecondaryQueuedChallenge(int id);
    void storeChallenge(int position, GJChallengeItem* challenge);
    void storeQueuedChallenge(int position, GJChallengeItem* challenge);
    void storeSecondaryQueuedChallenge(int position, GJChallengeItem* challenge);
    void removeChallenge(int position);
    void removeQueuedChallenge(int position);
    void removeQueuedSecondaryChallenge(int position);
    void processChallengeQueue(int position);
    void storeChallengeTime(int remaining);
    bool hasCompletedChallenge(GJChallengeItem* item);
    void resetChallengeTimer();
    bool areChallengesLoaded();

    bool claimListReward(GJLevelList* list);
    bool hasClaimedListReward(GJLevelList* list);
    std::string getListRewardKey(GJLevelList* list);

    GJRewardItem* createReward(GJRewardType type, int id, std::string str);
    GJRewardItem* getRewardItem(GJRewardType type);
    GJRewardItem* getRewardForSecretChest(int id);
    GJRewardItem* getRewardForSpecialChest(std::string key);
    std::string getRewardKey(GJRewardType type, int id);
    std::string getEventRewardKey(int id);
    std::string getSecretOnlineRewardKey(int id);
    std::string getGauntletRewardKey(int id);
    std::string getPathRewardKey(int id);
    std::string getItemKey(int id, int type);

    void storeOnlineChest(std::string key, GJRewardItem* item);
    void storeEventChest(int eventID, GJRewardItem* item);
    GJRewardItem* unlockOnlineChest(std::string key);
    GJRewardItem* unlockSecretChest(int id);
    GJRewardItem* unlockGoldChest(int id);
    GJRewardItem* unlockSpecialChest(std::string key);
    GJRewardItem* unlockGauntletChest(int id);
    GJRewardItem* unlockPathChest(int id);
    bool isSecretChestUnlocked(int id);
    bool isSpecialChestUnlocked(std::string key);
    bool isSpecialChestLiteUnlockable(std::string key);
    bool isGauntletChestUnlocked(int id);
    bool isPathChestUnlocked(int path);
    void resetSpecialChest(std::string key);
    void removeErrorFromSpecialChests();
    void linkSpecialChestUnlocks(GJRewardItem* item, std::string key);
    void processOnlineChests();
    int countSecretChests(GJRewardType rewardType);
    int countUnlockedSecretChests(GJRewardType rewardType);
    int keyCostForSecretChest(int id);
    int getNextGoldChestID();

    GJStoreItem* getStoreItem(int index);
    bool isStoreItemUnlocked(int index);
    bool purchaseItem(int index);
    ShopType shopTypeForItemID(int index);

    bool isItemUnlocked(UnlockType type, int id);
    bool isItemEnabled(UnlockType type, int id);
    int getItemUnlockState(int itemID, UnlockType unlockType);
    int getItemUnlockStateLite(int id, UnlockType type);
    bool canItemBeUnlocked(int id, UnlockType type);
    void toggleEnableItem(UnlockType type, int id, bool enabled);
    int getSecretChestForItem(int id, UnlockType type);
    cocos2d::CCString* getSpecialChestKeyForItem(int id, UnlockType type);
    bool isPathUnlocked(StatKey key);
    void updateActivePath(StatKey key);
    void trySelectActivePath();
    void unlockGauntlet(int id);
    bool isGauntletUnlocked(int id);

    std::string getSpecialRewardDescription(std::string key, bool unused);
    std::string getSpecialUnlockDescription(int id, UnlockType type, bool unused);
    void addSpecialRewardDescription(std::string key, std::string description);

    std::string usernameForAccountID(int id);
    int accountIDForIcon(int id, UnlockType type);

    void resetPreSync();
    void restorePostSync();
    void tempClear();

    void tryFixPathBug();

    bool areRewardsLoaded();
    std::string getNextVideoAdReward();

private:
    std::string getLevelKey(int levelID, bool isOnline, bool isDaily, bool isGauntlet, bool isEvent);

    cocos2d::CCDictionary* m_allStoreItems = nullptr;
    cocos2d::CCDictionary* m_storeItems = nullptr;
    cocos2d::CCArray* m_storeItemArray = nullptr;
    cocos2d::CCDictionary* m_purchasedItems = nullptr;

    cocos2d::CCDictionary* m_playerStats = nullptr;
    cocos2d::CCDictionary* m_completedLevels = nullptr;
    cocos2d::CCDictionary* m_completedMappacks = nullptr;
    cocos2d::CCDictionary* m_verifiedUserCoins = nullptr;
    cocos2d::CCDictionary* m_pendingUserCoins = nullptr;
    cocos2d::CCDictionary* m_completedLists = nullptr;
    cocos2d::CCDictionary* m_enabledItems = nullptr;
    cocos2d::CCDictionary* m_unlockedGauntlets = nullptr;
    cocos2d::CCDictionary* m_unlockedItems = nullptr;

    cocos2d::CCDictionary* m_onlineCurrencyScores = nullptr;
    cocos2d::CCDictionary* m_gauntletCurrencyScores = nullptr;
    cocos2d::CCDictionary* m_mainCurrencyScores = nullptr;
    cocos2d::CCDictionary* m_timelyCurrencyScores = nullptr;
    cocos2d::CCDictionary* m_onlineStars = nullptr;
    cocos2d::CCDictionary* m_timelyStars = nullptr;
    cocos2d::CCDictionary* m_gauntletDiamondScores = nullptr;
    cocos2d::CCDictionary* m_timelyDiamondScores = nullptr;
    cocos2d::CCDictionary* m_unusedCurrencyAwardDict = nullptr;

    cocos2d::CCDictionary* m_rewardItems = nullptr;
    cocos2d::CCDictionary* m_allSpecialChestItems = nullptr;
    cocos2d::CCDictionary* m_allTreasureRoomChestItems = nullptr;
    cocos2d::CCDictionary* m_allSpecialChests = nullptr;
    cocos2d::CCDictionary* m_allTreasureRoomChests = nullptr;
    cocos2d::CCDictionary* m_treasureRoomChests = nullptr;
    cocos2d::CCDictionary* m_miscChests = nullptr;
    cocos2d::CCDictionary* m_specialChestsLite = nullptr;
    cocos2d::CCDictionary* m_dailyChests = nullptr;
    cocos2d::CCDictionary* m_worldAdvertChests = nullptr;
    cocos2d::CCDictionary* m_weeklyChest = nullptr;
    cocos2d::CCDictionary* m_eventChest = nullptr;
    cocos2d::CCDictionary* m_wraithChests = nullptr;

    cocos2d::CCDictionary* m_activeChallenges = nullptr;
    cocos2d::CCDictionary* m_upcomingChallenges = nullptr;
    cocos2d::CCDictionary* m_challengeDiamonds = nullptr;
    double m_challengeTime = 0.0;

    cocos2d::CCString* m_trueString = nullptr;
    cocos2d::CCDictionary* m_unkDict = nullptr;

    struct RandomValue {
        int value;
        int seed;
        int random;
    };
    RandomValue m_bonusKey = {0, 0, 0};

    std::unordered_map<int, int> m_playerStatsRandMap;
    std::unordered_map<int, int> m_playerStatsSeedMap;
    bool m_usePlayerStatsCCDictionary = false;

    std::unordered_map<std::string, std::string> m_specialRewardDescriptions;
    std::unordered_map<std::string, std::string> m_createSpecialChestItemsMap;

    bool m_tryFixPathBug = false;
    bool m_pathBugFixed = false;
    bool m_skipIncrementChallenge = false;

    StatKey m_activePath = StatKey::FirePath;

    std::map<std::pair<int, UnlockType>, int> m_accountIDForIcon;
    std::map<int, std::string> m_usernameForAccountID;
    std::map<std::pair<UnlockType, int>, int> m_wraithIcons;
};