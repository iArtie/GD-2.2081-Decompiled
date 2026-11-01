#include "Managers/GameStatsManager.h"
#include "Models/GJGameLevel.h"
#include "Models/GJMapPack.h"
#include "Models/GJLevelList.h"
#include "Models/GJChallengeItem.h"
#include "Models/GJRewardItem.h"
#include "Models/GJStoreItem.h"
#include "Models/GJUnlockableItem.h"
#include "Utils/DS_Dictionary.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Managers/AchievementManager.h"
#include "Utils/LevelTools.h"
#include "Layers/GJPathsLayer.h"
#include "Nodes/GauntletNode.h"
#include "Utils/GameToolbox.h"

#include <ctime>

GameStatsManager* GameStatsManager::sharedState()
{
    static GameStatsManager* instance = nullptr;
    if (!instance)
    {
        instance = new GameStatsManager();
        instance->init();
    }
    return instance;
}

bool GameStatsManager::init()
{
    m_trueString = cocos2d::CCString::createWithFormat("%i", 1);
    m_trueString->retain();

    m_unkDict = cocos2d::CCDictionary::create();
    m_unkDict->retain();

    m_rewardItems = cocos2d::CCDictionary::create();
    m_rewardItems->retain();

    setupIconCredits();
    createStoreItems();
    createSecretChestItems();
    createSpecialChestItems();
    generateItemUnlockableData();

    return true;
}

GameStatsManager::~GameStatsManager()
{
    CC_SAFE_RELEASE(m_trueString);
    CC_SAFE_RELEASE(m_unkDict);
}

void GameStatsManager::firstSetup()
{
    m_playerStats = cocos2d::CCDictionary::create();
    m_playerStats->retain();

    m_completedLevels = cocos2d::CCDictionary::create();
    m_completedLevels->retain();

    m_verifiedUserCoins = cocos2d::CCDictionary::create();
    m_verifiedUserCoins->retain();

    m_pendingUserCoins = cocos2d::CCDictionary::create();
    m_pendingUserCoins->retain();

    m_completedMappacks = cocos2d::CCDictionary::create();
    m_completedMappacks->retain();

    m_purchasedItems = cocos2d::CCDictionary::create();
    m_purchasedItems->retain();

    m_onlineCurrencyScores = cocos2d::CCDictionary::create();
    m_onlineCurrencyScores->retain();

    m_gauntletCurrencyScores = cocos2d::CCDictionary::create();
    m_gauntletCurrencyScores->retain();

    m_unusedCurrencyAwardDict = cocos2d::CCDictionary::create();
    m_unusedCurrencyAwardDict->retain();

    m_onlineStars = cocos2d::CCDictionary::create();
    m_onlineStars->retain();

    m_mainCurrencyScores = cocos2d::CCDictionary::create();
    m_mainCurrencyScores->retain();

    m_timelyCurrencyScores = cocos2d::CCDictionary::create();
    m_timelyCurrencyScores->retain();

    m_timelyStars = cocos2d::CCDictionary::create();
    m_timelyStars->retain();

    m_gauntletDiamondScores = cocos2d::CCDictionary::create();
    m_gauntletDiamondScores->retain();

    m_timelyDiamondScores = cocos2d::CCDictionary::create();
    m_timelyDiamondScores->retain();

    m_dailyChests = cocos2d::CCDictionary::create();
    m_dailyChests->retain();

    m_worldAdvertChests = cocos2d::CCDictionary::create();
    m_worldAdvertChests->retain();

    m_weeklyChest = cocos2d::CCDictionary::create();
    m_weeklyChest->retain();

    m_eventChest = cocos2d::CCDictionary::create();
    m_eventChest->retain();

    m_activeChallenges = cocos2d::CCDictionary::create();
    m_activeChallenges->retain();

    m_upcomingChallenges = cocos2d::CCDictionary::create();
    m_upcomingChallenges->retain();

    m_challengeDiamonds = cocos2d::CCDictionary::create();
    m_challengeDiamonds->retain();

    m_completedLists = cocos2d::CCDictionary::create();
    m_completedLists->retain();

    m_treasureRoomChests = cocos2d::CCDictionary::create();
    m_treasureRoomChests->retain();

    m_miscChests = cocos2d::CCDictionary::create();
    m_miscChests->retain();

    m_enabledItems = cocos2d::CCDictionary::create();
    m_enabledItems->retain();

    m_wraithChests = cocos2d::CCDictionary::create();
    m_wraithChests->retain();

    m_unlockedGauntlets = cocos2d::CCDictionary::create();
    m_unlockedGauntlets->retain();

    m_bonusKey.value = 0;
    m_bonusKey.seed = rand() % 1000;
    m_bonusKey.random = m_bonusKey.seed;

    m_playerStatsRandMap.clear();
    m_playerStatsSeedMap.clear();
    m_pathBugFixed = false;
    m_activePath = StatKey::FirePath;
}

bool GameStatsManager::areChallengesLoaded()
{
    return m_challengeTime > 0.0;
}

bool GameStatsManager::areRewardsLoaded()
{
    return m_rewardItems->objectForKey(std::to_string(0).c_str()) != nullptr;
}

void GameStatsManager::resetChallengeTimer()
{
    m_challengeTime = 0.0;
}

bool GameStatsManager::shouldAwardSecretKey()
{
    return (float)(m_bonusKey.random - m_bonusKey.seed) < floorf(getStat("22") / 500.0f);
}

bool GameStatsManager::awardSecretKey()
{
    if (!shouldAwardSecretKey())
        return false;

    m_bonusKey.value = m_bonusKey.random - m_bonusKey.seed + 1;
    m_bonusKey.seed = rand() % 1000;
    m_bonusKey.random = m_bonusKey.value + m_bonusKey.seed;
    incrementStat("21");
    return true;
}

bool GameStatsManager::canItemBeUnlocked(int id, UnlockType type)
{
    return true;
}

void GameStatsManager::incrementStat(const char* key)
{
    incrementStat(key, 1);
}

void GameStatsManager::incrementStat(const char* key, int amount)
{
    setStat(key, getStat(key) + amount);
}

void GameStatsManager::setStat(const char* key, int value)
{
    if (m_usePlayerStatsCCDictionary)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", value);
        m_playerStats->setObject(str, key);
    }
    else
    {
        int randomVal = rand() % 10000;
        int keyInt = atoi(key);
        m_playerStatsRandMap[keyInt] = value + randomVal;
        m_playerStatsSeedMap[keyInt] = randomVal;
    }
    checkAchievement(key);
}

void GameStatsManager::setStatIfHigher(const char* key, int value)
{
    if (getStat(key) < value)
        setStat(key, value);
}

int GameStatsManager::getStat(const char* key)
{
    if (m_usePlayerStatsCCDictionary)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)m_playerStats->valueForKey(key);
        return str ? str->intValue() : 0;
    }
    else
    {
        int keyInt = atoi(key);
        return m_playerStatsRandMap[keyInt] - m_playerStatsSeedMap[keyInt];
    }
}

int GameStatsManager::getStatFromKey(StatKey key)
{
    std::string keyStr = GameToolbox::intToString((int)key);
    return getStat(keyStr.c_str());
}

void GameStatsManager::incrementChallenge(GJChallengeType type, int amount)
{
    for (int i = 1; i <= 3; i++)
    {
        GJChallengeItem* challenge = getChallenge(i);
        if (!challenge)
            continue;

        if (!challenge->m_canClaim && challenge->m_challengeType == type)
        {
            GJChallengeItem::incrementCount(challenge, amount);

            if (challenge->m_canClaim)
            {
                std::string description = "";
                std::string iconName = "";
                switch (challenge->m_challengeType)
                {
                    case GJChallengeType::Orbs:
                        description = "Mana Orbs";
                        iconName = "currencyOrbIcon_001.png";
                        break;
                    case GJChallengeType::Stars:
                        description = "Stars/Moons";
                        iconName = "GJ_starsIcon_001.png";
                        break;
                    case GJChallengeType::Coins:
                        if (challenge->m_goal.random - challenge->m_goal.seed == 1)
                        {
                            description = "Silver Coin";
                            iconName = "GJ_coinsIcon2_001.png";
                        }
                        else
                        {
                            description = "Silver Coins";
                            iconName = "GJ_coinsIcon2_001.png";
                        }
                        break;
                    default:
                        description = "?";
                        break;
                }

                AchievementManager* am = AchievementManager::sharedState();
                cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
                    "Collect %i %s.",
                    challenge->m_goal.random - challenge->m_goal.seed,
                    description.c_str());
                AchievementManager::notifyAchievement(am, "Quest Complete!", str->getCString(), iconName.c_str());
            }
        }
    }
}

void GameStatsManager::incrementActivePath(int amount)
{
    trySelectActivePath();
    if ((int)m_activePath >= 30 && (int)m_activePath <= 39)
    {
        std::string keyStr = GameToolbox::intToString((int)m_activePath);
        incrementStat(keyStr.c_str(), amount);
    }
}

void GameStatsManager::trySelectActivePath()
{
    updateActivePath(m_activePath);

    if (m_activePath != StatKey::FirePath)
    {
        if (!isPathUnlocked(m_activePath))
        {
            m_activePath = StatKey::FirePath;
        }
        else if (getStat(GameToolbox::intToString((int)m_activePath).c_str()) > 999)
        {
            m_activePath = StatKey::FirePath;
        }
        else
        {
            return;
        }
    }

    for (int path = (int)StatKey::FirePath; path < (int)StatKey::Gauntlets; path++)
    {
        if (isPathUnlocked((StatKey)path))
        {
            std::string keyStr = GameToolbox::intToString(path);
            if (getStat(keyStr.c_str()) <= 999)
            {
                m_activePath = (StatKey)path;
                return;
            }
        }
    }
}

void GameStatsManager::updateActivePath(StatKey key)
{
    if ((int)key < 30 || (int)key >= 40)
        key = StatKey::FirePath;
    m_activePath = key;
}

bool GameStatsManager::isPathUnlocked(StatKey key)
{
    return isItemUnlocked(UnlockType::GJItem, (int)key);
}

int GameStatsManager::getBaseCurrency(int stars, bool mainLevel, int levelID)
{
    if (mainLevel)
    {
        if (levelID == 18 || levelID == 20 || levelID == 14)
            return 400;
        return 20 * (stars + 1);
    }
    else
    {
        int index = stars - 2;
        if (index >= 0 && index <= 8)
        {
            static int baseCurrencyTable[] = {10, 20, 30, 40, 50, 60, 70, 80, 100};
            return baseCurrencyTable[index];
        }
        return 0;
    }
}

int GameStatsManager::getBaseCurrencyForLevel(GJGameLevel* level)
{
    bool isMain = level->m_levelType == LevelType::Main;
    int stars = level->m_stars.random - level->m_stars.seed;

    if (level->m_levelType != LevelType::Main)
    {
        std::string key = getCurrencyKey(level);
        bool isDaily = level->m_dailyID.random - level->m_dailyID.seed <= 0;
        cocos2d::CCDictionary* targetDict = isDaily ? m_onlineStars : m_timelyStars;
        cocos2d::CCString* str = (cocos2d::CCString*)targetDict->valueForKey(key.c_str());
        int storedStars = str ? str->intValue() : 0;
        if (stars < storedStars)
            stars = storedStars;
    }

    return getBaseCurrency(stars, isMain, level->m_levelID.random - level->m_levelID.seed);
}

int GameStatsManager::getBaseDiamonds(int stars)
{
    int index = stars - 2;
    if (index < 0 || index > 8)
        return 0;
    return stars + 2;
}

int GameStatsManager::getBonusDiamonds(int stars)
{
    int index = stars - 2;
    if (index < 0 || index > 8)
        return 0;
    if (stars == 10)
        return 20;
    return (int)(getBaseDiamonds(stars) * 0.5f);
}

void GameStatsManager::awardCurrencyForLevel(GJGameLevel* level)
{
    bool isDaily = level->m_dailyID.random - level->m_dailyID.seed > 0;
    int stars = level->m_stars.random - level->m_stars.seed;
    bool isMain = level->m_levelType == LevelType::Main;
    int orbCompletion = level->m_orbCompletion.random - level->m_orbCompletion.seed;
    bool isGauntlet = level->m_gauntletLevel;

    if (!isDaily)
    {
        if (isMain)
        {
            m_timelyCurrencyScores = m_mainCurrencyScores;
        }
        else if (isGauntlet)
        {
            m_timelyCurrencyScores = m_gauntletCurrencyScores;
        }
        else
        {
            m_timelyCurrencyScores = m_onlineCurrencyScores;
        }
    }
    else
    {
        m_timelyCurrencyScores = m_timelyCurrencyScores;
    }

    if (orbCompletion <= 0)
        return;

    if (stars > 0)
    {
        int newPercent = 100;
        if (orbCompletion <= 99)
            newPercent = orbCompletion;

        std::string key = getCurrencyKey(level);
        cocos2d::CCString* str = (cocos2d::CCString*)m_timelyCurrencyScores->valueForKey(key.c_str());
        int oldPercent = str ? str->intValue() : 0;

        if (newPercent > oldPercent)
        {
            int baseCurrency = getBaseCurrencyForLevel(level);
            int oldCurrency = (int)(baseCurrency * ((float)oldPercent / 100.0f));
            int newCurrency = (int)(baseCurrency * ((float)newPercent / 100.0f));

            if (newPercent == 100)
                newCurrency = (int)(newCurrency + baseCurrency * 0.25f);

            int difference = newCurrency - oldCurrency;

            if (difference > 0)
            {
                cocos2d::CCString* newStr = cocos2d::CCString::createWithFormat("%i", newPercent);
                m_timelyCurrencyScores->setObject(newStr, key.c_str());

                if (!isMain)
                {
                    bool isOnlineOnly = !isDaily;
                    cocos2d::CCDictionary* starDict = isOnlineOnly ? m_onlineStars : m_timelyStars;
                    cocos2d::CCString* starStr = (cocos2d::CCString*)starDict->valueForKey(key.c_str());
                    int starVal = starStr ? starStr->intValue() : 0;

                    if (starVal < stars)
                    {
                        cocos2d::CCString* newStarStr = cocos2d::CCString::createWithFormat("%i", stars);
                        starDict->setObject(newStarStr, key.c_str());
                    }
                }

                incrementStat("14", difference);
            }
        }
    }
}

void GameStatsManager::awardDiamondsForLevel(GJGameLevel* level)
{
    bool isDaily = level->m_dailyID.random - level->m_dailyID.seed > 0;
    bool isGauntlet = level->m_gauntletLevel;

    if (!isDaily && !isGauntlet)
        return;

    int stars = level->m_stars.random - level->m_stars.seed;
    int orbCompletion = level->m_orbCompletion.random - level->m_orbCompletion.seed;

    if (stars <= 0 || orbCompletion <= 0)
        return;

    int baseDiamonds = getBaseDiamonds(stars);
    if (baseDiamonds <= 0)
        return;

    int newPercent = 100;
    if (orbCompletion <= 99)
        newPercent = orbCompletion;

    std::string key = getCurrencyKey(level);
    
    if (!isDaily)
        m_timelyDiamondScores = m_gauntletDiamondScores;

    cocos2d::CCString* str = (cocos2d::CCString*)m_timelyDiamondScores->valueForKey(key.c_str());
    int oldPercent = str ? str->intValue() : 0;

    if (newPercent > oldPercent)
    {
        int oldDiamonds = (int)(baseDiamonds * ((float)oldPercent / 100.0f));
        int newDiamonds = (int)(baseDiamonds * ((float)newPercent / 100.0f));
        int difference = newDiamonds - oldDiamonds;

        if (difference > 0)
        {
            cocos2d::CCString* newStr = cocos2d::CCString::createWithFormat("%i", newPercent);
            m_timelyDiamondScores->setObject(newStr, key.c_str());

            cocos2d::CCDictionary* starDict = (!isDaily) ? m_onlineStars : m_timelyStars;
            cocos2d::CCString* starStr = (cocos2d::CCString*)starDict->valueForKey(key.c_str());
            int starVal = starStr ? starStr->intValue() : 0;

            if (starVal < stars)
            {
                cocos2d::CCString* newStarStr = cocos2d::CCString::createWithFormat("%i", stars);
                starDict->setObject(newStarStr, key.c_str());
            }

            incrementStat("13", difference);
        }
    }
}

int GameStatsManager::getAwardedCurrencyForLevel(GJGameLevel* level)
{
    bool isDaily = level->m_dailyID.random - level->m_dailyID.seed > 0;
    bool isMain = level->m_levelType == LevelType::Main;
    bool isGauntlet = level->m_gauntletLevel;

    cocos2d::CCDictionary* targetDict;
    if (isDaily)
        targetDict = m_timelyCurrencyScores;
    else if (isMain)
        targetDict = m_mainCurrencyScores;
    else if (isGauntlet)
        targetDict = m_gauntletCurrencyScores;
    else
        targetDict = m_onlineCurrencyScores;

    std::string key = getCurrencyKey(level);
    cocos2d::CCString* str = (cocos2d::CCString*)targetDict->valueForKey(key.c_str());
    int percent = str ? str->intValue() : 0;

    int baseCurrency = getBaseCurrencyForLevel(level);

    if (percent > 99)
        return baseCurrency + (int)(baseCurrency * 0.25f);
    else
        return (int)(baseCurrency * ((float)percent / 100.0f));
}

int GameStatsManager::getAwardedDiamondsForLevel(GJGameLevel* level)
{
    bool isDaily = level->m_dailyID.random - level->m_dailyID.seed > 0;
    bool isGauntlet = level->m_gauntletLevel;

    if (!isDaily && !isGauntlet)
        return 0;

    std::string key = getCurrencyKey(level);
    
    if (!isDaily)
        m_timelyDiamondScores = m_gauntletDiamondScores;

    cocos2d::CCString* str = (cocos2d::CCString*)m_timelyDiamondScores->valueForKey(key.c_str());
    int percent = str ? str->intValue() : 0;

    int baseDiamonds = getBaseDiamonds(level->m_stars.random - level->m_stars.seed);
    return (int)(baseDiamonds * ((float)percent / 100.0f));
}

int GameStatsManager::getTotalCollectedCurrency()
{
    float total = 0.0f;

    // Main levels
    for (int i = 1; i <= 22; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, true);
        if (level)
            total += getAwardedCurrencyForLevel(level);
        else
            m_mainCurrencyScores->removeObjectForKey(std::to_string(i).c_str());
    }

    // Tower levels
    for (int i = 5001; i <= 5004; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, true);
        if (level)
            total += getAwardedCurrencyForLevel(level);
        else
            m_mainCurrencyScores->removeObjectForKey(std::to_string(i).c_str());
    }

    // Map pack levels
    for (int i = 2001; i <= 2010; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, true);
        if (level)
            total += getAwardedCurrencyForLevel(level);
        else
            m_mainCurrencyScores->removeObjectForKey(std::to_string(i).c_str());
    }

    // Online levels
    cocos2d::CCArray* onlineKeys = m_onlineCurrencyScores->allKeys();
    for (int i = 0; i < onlineKeys->count(); i++)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)onlineKeys->objectAtIndex(i);
        int levelID = str->intValue();
        GJGameLevel* level = GameLevelManager::sharedState()->getSavedLevel(levelID);
        if (level)
            total += getAwardedCurrencyForLevel(level);
        else
            m_onlineCurrencyScores->removeObjectForKey(std::to_string(levelID).c_str());
    }

    // Gauntlet levels
    cocos2d::CCArray* gauntletKeys = m_gauntletCurrencyScores->allKeys();
    for (int i = 0; i < gauntletKeys->count(); i++)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)gauntletKeys->objectAtIndex(i);
        int levelID = str->intValue();
        GJGameLevel* level = GameLevelManager::sharedState()->getSavedGauntletLevel(levelID);
        if (level)
            total += getAwardedCurrencyForLevel(level);
        else
            m_gauntletCurrencyScores->removeObjectForKey(std::to_string(levelID).c_str());
    }

    // Daily levels
    cocos2d::CCArray* dailyKeys = m_timelyCurrencyScores->allKeys();
    for (int i = 0; i < dailyKeys->count(); i++)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)dailyKeys->objectAtIndex(i);
        int dailyID = str->intValue();
        GJGameLevel* level = GameLevelManager::sharedState()->getSavedDailyLevel(dailyID);
        if (level)
            total += getAwardedCurrencyForLevel(level);
        else
        {
            m_timelyCurrencyScores->removeObjectForKey(std::to_string(dailyID).c_str());
            m_timelyStars->removeObjectForKey(std::to_string(dailyID).c_str());
        }
    }

    return (int)total;
}

int GameStatsManager::getTotalCollectedDiamonds()
{
    int total = 0;

    // Challenge diamonds
    if (m_challengeDiamonds)
    {
        cocos2d::CCDictElement* element = m_challengeDiamonds->m_pElements;
        while (element)
        {
            cocos2d::CCString* str = (cocos2d::CCString*)element->m_pObject;
            total += str->intValue();
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }

    // List rewards
    if (m_completedLists)
    {
        cocos2d::CCDictElement* element = m_completedLists->m_pElements;
        while (element)
        {
            cocos2d::CCString* str = (cocos2d::CCString*)element->m_pObject;
            total += str->intValue();
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }

    // Gauntlet diamonds
    cocos2d::CCArray* gauntletKeys = m_gauntletDiamondScores->allKeys();
    for (int i = 0; i < gauntletKeys->count(); i++)
    {
        cocos2d::CCString* key = (cocos2d::CCString*)gauntletKeys->objectAtIndex(i);
        cocos2d::CCString* percentStr = (cocos2d::CCString*)m_gauntletDiamondScores->valueForKey(key->getCString());
        int percent = percentStr ? percentStr->intValue() : 0;
        
        cocos2d::CCString* starStr = (cocos2d::CCString*)m_onlineStars->valueForKey(key->getCString());
        int stars = starStr ? starStr->intValue() : 0;
        
        total += (int)(getBaseDiamonds(stars) * ((float)percent / 100.0f));
    }

    return total;
}

std::string GameStatsManager::getLevelKey(GJGameLevel* level)
{
    int dailyID = level->m_dailyID.random - level->m_dailyID.seed;
    int levelID = level->m_levelID.random - level->m_levelID.seed;
    bool isOnline = dailyID <= 0 && level->m_levelType != LevelType::Main;
    bool isDaily = dailyID > 0;
    bool isGauntlet = level->m_gauntletLevel;
    bool isEvent = dailyID > 200000;

    return getLevelKey(levelID, isOnline, isDaily, isGauntlet, isEvent);
}

std::string GameStatsManager::getLevelKey(int levelID, bool isOnline, bool isDaily, bool isGauntlet, bool isEvent)
{
    if (isDaily)
        return "daily_" + std::to_string(levelID);
    if (isEvent)
        return "event_" + std::to_string(levelID);
    if (isGauntlet)
        return "gauntlet_" + std::to_string(levelID);
    if (isOnline)
        return "online_" + std::to_string(levelID);
    return std::to_string(levelID);
}

std::string GameStatsManager::getCurrencyKey(GJGameLevel* level)
{
    int dailyID = level->m_dailyID.random - level->m_dailyID.seed;
    if (dailyID > 0)
        return std::to_string(dailyID);
    else
        return std::to_string(level->m_levelID.random - level->m_levelID.seed);
}

std::string GameStatsManager::getDailyLevelKey(int dailyID)
{
    return "d" + std::to_string(dailyID);
}

const char* GameStatsManager::getDemonLevelKey(GJGameLevel* level)
{
    int dailyID = level->m_dailyID.random - level->m_dailyID.seed;
    if (dailyID > 0)
    {
        return cocos2d::CCString::createWithFormat("ddemon_%i", dailyID)->getCString();
    }
    else
    {
        int levelID = level->m_levelID.random - level->m_levelID.seed;
        if (level->m_gauntletLevel)
            return cocos2d::CCString::createWithFormat("gdemon_%i", levelID)->getCString();
        else
            return cocos2d::CCString::createWithFormat("demon_%i", levelID)->getCString();
    }
}

const char* GameStatsManager::getStarLevelKey(GJGameLevel* level)
{
    int dailyID = level->m_dailyID.random - level->m_dailyID.seed;
    if (dailyID > 0)
    {
        return cocos2d::CCString::createWithFormat("dstar_%i", dailyID)->getCString();
    }
    else
    {
        int levelID = level->m_levelID.random - level->m_levelID.seed;
        if (level->m_gauntletLevel)
            return cocos2d::CCString::createWithFormat("gstar_%i", levelID)->getCString();
        else
            return cocos2d::CCString::createWithFormat("star_%i", levelID)->getCString();
    }
}

const char* GameStatsManager::getMapPackKey(int id)
{
    return cocos2d::CCString::createWithFormat("pack_%i", id)->getCString();
}

const char* GameStatsManager::getSecretCoinKey(const char* key)
{
    return cocos2d::CCString::createWithFormat("unique_%s", key)->getCString();
}

std::string GameStatsManager::getItemKey(int id, int type)
{
    return std::to_string(id) + "_" + std::to_string(type);
}

std::string GameStatsManager::getGauntletRewardKey(int id)
{
    return "g_" + std::to_string(id);
}

std::string GameStatsManager::getPathRewardKey(int id)
{
    return "pr_" + std::to_string(id);
}

std::string GameStatsManager::getListRewardKey(GJLevelList* list)
{
    return "lr_" + std::to_string(list->m_listID);
}

std::string GameStatsManager::getRewardKey(GJRewardType type, int id)
{
    return std::to_string((int)type) + "_" + std::to_string(id);
}

std::string GameStatsManager::getEventRewardKey(int id)
{
    return "o_event_" + std::to_string(id);
}

std::string GameStatsManager::getSecretOnlineRewardKey(int id)
{
    return "o_secret_" + std::to_string(id);
}

std::string GameStatsManager::getChallengeKey(GJChallengeItem* chal)
{
    return "c" + std::to_string(chal->m_position) + std::to_string(chal->m_timeLeft);
}

bool GameStatsManager::hasCompletedLevel(GJGameLevel* level)
{
    std::string key = getLevelKey(level);
    return m_completedLevels->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::hasCompletedMainLevel(int levelID)
{
    std::string key = std::to_string(levelID);
    return m_completedLevels->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::hasCompletedOnlineLevel(int id)
{
    std::string key = "online_" + std::to_string(id);
    return m_completedLevels->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::hasCompletedGauntletLevel(int id)
{
    std::string key = "gauntlet_" + std::to_string(id);
    return m_completedLevels->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::hasCompletedStarLevel(GJGameLevel* level)
{
    const char* key = getStarLevelKey(level);
    return m_completedLevels->objectForKey(key) != nullptr;
}

bool GameStatsManager::hasCompletedDemonLevel(GJGameLevel* level)
{
    const char* key = getDemonLevelKey(level);
    return m_completedLevels->objectForKey(key) != nullptr;
}

bool GameStatsManager::hasCompletedMapPack(int id)
{
    const char* key = getMapPackKey(id);
    return m_completedLevels->objectForKey(key) != nullptr;
}

bool GameStatsManager::hasCompletedDailyLevel(int dailyID)
{
    std::string key = getDailyLevelKey(dailyID);
    
    if (dailyID <= 99999)
        return m_challengeDiamonds->objectForKey(key.c_str()) != nullptr;
    else if (dailyID <= 199999)
        return m_weeklyChest->objectForKey(key.c_str()) != nullptr;
    else
        return m_wraithChests->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::hasCompletedChallenge(GJChallengeItem* item)
{
    std::string key = getChallengeKey(item);
    return m_challengeDiamonds->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::hasClaimedListReward(GJLevelList* list)
{
    std::string key = getListRewardKey(list);
    return m_completedLists->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::hasUserCoin(const char* key)
{
    return m_verifiedUserCoins->objectForKey(key) != nullptr;
}

bool GameStatsManager::hasPendingUserCoin(const char* key)
{
    return m_pendingUserCoins->objectForKey(key) != nullptr;
}

bool GameStatsManager::hasSecretCoin(const char* key)
{
    const char* secretKey = getSecretCoinKey(key);
    return m_playerStats->objectForKey(secretKey) != nullptr;
}

void GameStatsManager::storeUserCoin(const char* key)
{
    m_verifiedUserCoins->setObject(m_trueString, key);
}

void GameStatsManager::storePendingUserCoin(const char* key)
{
    m_pendingUserCoins->setObject(m_trueString, key);
}

void GameStatsManager::storeSecretCoin(const char* key)
{
    const char* secretKey = getSecretCoinKey(key);
    m_playerStats->setObject(m_trueString, secretKey);
}

void GameStatsManager::resetUserCoins()
{
    setStat("12", 0);
    m_verifiedUserCoins->removeAllObjects();
    m_pendingUserCoins->removeAllObjects();
}

bool GameStatsManager::isSecretCoin(std::string key)
{
    return key.find("unique_") == 0;
}

int GameStatsManager::getCollectedCoinsForLevel(GJGameLevel* level)
{
    int collected = 0;
    for (int i = 1; i <= 3; i++)
    {
        const char* coinKey = GJGameLevel::getCoinKey(level, i);
        if (hasSecretCoin(coinKey))
            collected++;
    }
    return collected;
}

void GameStatsManager::logCoins()
{
    cocos2d::CCArray* keys = m_verifiedUserCoins->allKeys();
    for (int i = 0; i < keys->count(); i++)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)keys->objectAtIndex(i);
        str->getCString();
    }
}

int GameStatsManager::starsForMapPack(int id)
{
    const char* key = getMapPackKey(id);
    cocos2d::CCString* str = (cocos2d::CCString*)m_completedMappacks->valueForKey(key);
    return str ? str->intValue() : 0;
}

void GameStatsManager::setStarsForMapPack(int id, int stars)
{
    const char* key = getMapPackKey(id);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", stars);
    m_completedMappacks->setObject(str, key);
}

cocos2d::CCArray* GameStatsManager::getCompletedMapPacks()
{
    cocos2d::CCArray* allKeys = m_completedLevels->allKeys();
    cocos2d::CCArray* result = cocos2d::CCArray::create();

    for (int i = 0; i < allKeys->count(); i++)
    {
        cocos2d::CCString* key = (cocos2d::CCString*)allKeys->objectAtIndex(i);
        std::string keyStr = key->getCString();
        
        size_t found = keyStr.find("pack_");
        if (found != std::string::npos)
        {
            std::string packID = keyStr.substr(found + 5);
            result->addObject(cocos2d::CCString::create(packID.c_str()));
        }
    }

    return result;
}

void GameStatsManager::completedLevel(GJGameLevel* level)
{
    std::string key = getLevelKey(level);
    
    if (!hasCompletedLevel(level))
    {
        m_completedLevels->setObject(m_trueString, key.c_str());
        
        int dailyID = level->m_dailyID.random - level->m_dailyID.seed;
        if (dailyID >= 1 && dailyID <= 99999)
        {
            incrementStat("15");
        }
        else if (level->m_levelType == LevelType::Main)
        {
            incrementStat("3");
        }
        else
        {
            incrementStat("4");
        }
    }

    if (level->m_demon.random - level->m_demon.seed == 1)
        completedDemonLevel(level);

    if (level->m_stars.random - level->m_stars.seed > 0)
        completedStarLevel(level);
}

void GameStatsManager::completedDemonLevel(GJGameLevel* level)
{
    if (hasCompletedDemonLevel(level))
        return;
    
    if (level->m_demon.random - level->m_demon.seed != 1)
        return;

    const char* key = getDemonLevelKey(level);
    m_completedLevels->setObject(m_trueString, key);

    if (level->m_stars.random - level->m_stars.seed > 9)
        incrementStat("5");
}

void GameStatsManager::completedStarLevel(GJGameLevel* level)
{
    if (hasCompletedStarLevel(level))
        return;
    
    if (level->m_stars.random - level->m_stars.seed <= 0)
        return;

    const char* key = getStarLevelKey(level);
    m_completedLevels->setObject(m_trueString, key);

    int stars = level->m_stars.random - level->m_stars.seed;
    bool isMain = level->m_levelType == LevelType::Main;
    bool isDemon = level->m_levelLength == 5;

    if (!isMain && isDemon)
        stars = 0;
    else if (isDemon)
        stars = 0;

    if (!isDemon)
    {
        incrementStat("6", stars);
        if (stars >= 9 && stars <= 10)
            incrementStat("42");
    }
    else
    {
        incrementStat("28", stars);
        if (stars >= 9 && stars <= 10)
            incrementStat("42");
    }
}

void GameStatsManager::completedMapPack(GJMapPack* pack)
{
    if (hasCompletedMapPack(pack->m_packID))
        return;

    const char* key = getMapPackKey(pack->m_packID);
    m_completedLevels->setObject(m_trueString, key);

    if (GJMapPack::totalMaps(pack) > 2)
    {
        int stars = pack->m_stars >= 11 ? 0 : pack->m_stars;
        int coins = pack->m_coins >= 3 ? 0 : pack->m_coins;
        
        incrementStat("6", stars);
        incrementStat("8", coins);
        incrementStat("7");
        setStarsForMapPack(pack->m_packID, stars);
    }
}

void GameStatsManager::markLevelAsCompletedAndClaimed(GJGameLevel* level)
{
    std::string key = getLevelKey(level);
    m_completedLevels->setObject(m_trueString, key.c_str());

    const char* starKey = getStarLevelKey(level);
    m_completedLevels->setObject(m_trueString, starKey);

    const char* demonKey = getDemonLevelKey(level);
    m_completedLevels->setObject(m_trueString, demonKey);
}

void GameStatsManager::uncompleteLevel(GJGameLevel* level)
{
    std::string key = getLevelKey(level);
    m_completedLevels->removeObjectForKey(key.c_str());

    const char* starKey = getStarLevelKey(level);
    m_completedLevels->removeObjectForKey(starKey);

    const char* demonKey = getDemonLevelKey(level);
    m_completedLevels->removeObjectForKey(demonKey);
}

void GameStatsManager::checkAchievement(const char* statKey)
{
    int key = atoi(statKey);
    
    switch (key)
    {
        case 1: // Jumps
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.jump01", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump02", (int)std::floor((stat / 10000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump03", (int)std::floor((stat / 20000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump04", (int)std::floor((stat / 50000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump05", (int)std::floor((stat / 100000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump06", (int)std::floor((stat / 200000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump07", (int)std::floor((stat / 300000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump08", (int)std::floor((stat / 400000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump09", (int)std::floor((stat / 500000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump10", (int)std::floor((stat / 750000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.jump11", (int)std::floor((stat / 1000000.0f) * 100.0f), 0);
            break;
        }
        case 2: // Attempts
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.attempt01", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt02", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt03", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt04", (int)std::floor((stat / 10000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt05", (int)std::floor((stat / 20000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt06", (int)std::floor((stat / 30000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt07", (int)std::floor((stat / 40000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt08", (int)std::floor((stat / 60000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt09", (int)std::floor((stat / 80000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt10", (int)std::floor((stat / 100000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt11", (int)std::floor((stat / 135000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt12", (int)std::floor((stat / 185000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt13", (int)std::floor((stat / 250000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.attempt14", (int)std::floor((stat / 300000.0f) * 100.0f), 0);
            break;
        }
        case 4: // Completed Online
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.custom01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.custom02", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom03", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom04", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom05", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom06", (int)std::floor((stat / 300.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom07", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom08", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom09", (int)std::floor((stat / 1500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom10", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom11", (int)std::floor((stat / 2500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom12", (int)std::floor((stat / 3000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom13", (int)std::floor((stat / 4000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.custom14", (int)std::floor((stat / 5000.0f) * 100.0f), 0);
            break;
        }
        case 5: // Demons
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.demon01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.demon02", (int)std::floor((stat * 0.5f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon03", (int)std::floor((stat / 3.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon04", (int)std::floor((stat * 0.25f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon05", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon06", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon07", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon08", (int)std::floor((stat / 20.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon09", (int)std::floor((stat / 30.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon10", (int)std::floor((stat / 40.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon11", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon12", (int)std::floor((stat / 60.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon13", (int)std::floor((stat / 70.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon14", (int)std::floor((stat / 80.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon15", (int)std::floor((stat / 90.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon16", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon17", (int)std::floor((stat / 120.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon18", (int)std::floor((stat / 140.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon19", (int)std::floor((stat / 160.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon20", (int)std::floor((stat / 180.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon21", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon22", (int)std::floor((stat / 225.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon23", (int)std::floor((stat / 250.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon24", (int)std::floor((stat / 300.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon25", (int)std::floor((stat / 350.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon26", (int)std::floor((stat / 400.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.demon27", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            break;
        }
        case 6: // Stars
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.stars01", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars02", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars03", (int)std::floor((stat / 300.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars04", (int)std::floor((stat / 400.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars05", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars06", (int)std::floor((stat / 600.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars07", (int)std::floor((stat / 700.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars08", (int)std::floor((stat / 800.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars09", (int)std::floor((stat / 900.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars10", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars11", (int)std::floor((stat / 1500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars12", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars13", (int)std::floor((stat / 2500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars14", (int)std::floor((stat / 3000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars15", (int)std::floor((stat / 3500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars16", (int)std::floor((stat / 4000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars17", (int)std::floor((stat / 4500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars18", (int)std::floor((stat / 5000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars19", (int)std::floor((stat / 5500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars20", (int)std::floor((stat / 6000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars21", (int)std::floor((stat / 6500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars22", (int)std::floor((stat / 7000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars23", (int)std::floor((stat / 7500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars24", (int)std::floor((stat / 8000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars25", (int)std::floor((stat / 9000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars26", (int)std::floor((stat / 10000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars27", (int)std::floor((stat / 12000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars28", (int)std::floor((stat / 14000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars29", (int)std::floor((stat / 16000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars30", (int)std::floor((stat / 18000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars31", (int)std::floor((stat / 20000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars32", (int)std::floor((stat / 22500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.stars33", (int)std::floor((stat / 25000.0f) * 100.0f), 0);
            break;
        }
        case 7: // Map Packs
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.mappacks01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.mappacks02", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks03", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks04", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks05", (int)std::floor((stat / 20.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks06", (int)std::floor((stat / 25.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks07", (int)std::floor((stat / 30.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks08", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks09", (int)std::floor((stat / 40.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.mappacks10", (int)std::floor((stat / 45.0f) * 100.0f), 0);
            break;
        }
        case 8: // Secret Coins
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.coins01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins02", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins03", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins04", (int)std::floor((stat / 20.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins05", (int)std::floor((stat / 25.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins06", (int)std::floor((stat / 30.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins07", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins08", (int)std::floor((stat / 40.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins09", (int)std::floor((stat / 45.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins10", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins11", (int)std::floor((stat / 55.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins12", (int)std::floor((stat / 60.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins13", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins14", (int)std::floor((stat / 70.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins15", (int)std::floor((stat / 75.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins16", (int)std::floor((stat / 80.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins17", (int)std::floor((stat / 85.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins18", (int)std::floor((stat / 90.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins19", (int)std::floor((stat / 95.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins20", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins21", (int)std::floor((stat / 105.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins22", (int)std::floor((stat / 110.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins23", (int)std::floor((stat / 115.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins24", (int)std::floor((stat / 120.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins25", (int)std::floor((stat / 130.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins26", (int)std::floor((stat / 140.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins27", (int)std::floor((stat / 150.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.coins28", (int)std::floor((stat / 160.0f) * 100.0f), 0);
            break;
        }
        case 9: // Secret Coins (alternate)
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.secret01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.secret02", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.secret02b", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.secret03", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.secret03b", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.secret18", (int)std::floor((stat / 750.0f) * 100.0f), 0);
            break;
        }
        case 10: // Likes
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.like02", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.like02b", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.like03", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.like04", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.like05", (int)std::floor((stat / 3000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.like06", (int)std::floor((stat / 4000.0f) * 100.0f), 0);
            break;
        }
        case 11: // Rate Diff
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.rateDiff02", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.rateDiff02b", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.rateDiff03", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.rateDiff04", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            break;
        }
        case 12: // User Coins
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.usercoins01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.usercoins02", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins03", (int)std::floor((stat / 20.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins04", (int)std::floor((stat / 30.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins05", (int)std::floor((stat / 40.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins06", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins07", (int)std::floor((stat / 60.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins08", (int)std::floor((stat / 70.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins09", (int)std::floor((stat / 80.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins10", (int)std::floor((stat / 90.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins11", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins12", (int)std::floor((stat / 110.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins13", (int)std::floor((stat / 120.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins14", (int)std::floor((stat / 130.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins15", (int)std::floor((stat / 140.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins16", (int)std::floor((stat / 150.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins17", (int)std::floor((stat / 160.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins18", (int)std::floor((stat / 170.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins19", (int)std::floor((stat / 180.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins20", (int)std::floor((stat / 190.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins21", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins22", (int)std::floor((stat / 225.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins23", (int)std::floor((stat / 250.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins24", (int)std::floor((stat / 300.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins25", (int)std::floor((stat / 350.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins26", (int)std::floor((stat / 425.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins27", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins28", (int)std::floor((stat / 600.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins29", (int)std::floor((stat / 700.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins30", (int)std::floor((stat / 800.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins31", (int)std::floor((stat / 900.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins32", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins33", (int)std::floor((stat / 1200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins34", (int)std::floor((stat / 1400.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins35", (int)std::floor((stat / 1600.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins36", (int)std::floor((stat / 1800.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins37", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins38", (int)std::floor((stat / 2300.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins39", (int)std::floor((stat / 2600.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.usercoins40", (int)std::floor((stat / 3000.0f) * 100.0f), 0);
            break;
        }
        case 13: // Diamonds
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.diamonds01", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds02", (int)std::floor((stat / 250.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds03", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds04", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds05", (int)std::floor((stat / 1500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds06", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds07", (int)std::floor((stat / 2500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds08", (int)std::floor((stat / 3000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds09", (int)std::floor((stat / 4000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds10", (int)std::floor((stat / 5000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds11", (int)std::floor((stat / 6000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds12", (int)std::floor((stat / 7000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds13", (int)std::floor((stat / 9000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds14", (int)std::floor((stat / 11000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds15", (int)std::floor((stat / 14000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds16", (int)std::floor((stat / 17000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds17", (int)std::floor((stat / 20000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.diamonds18", (int)std::floor((stat / 25000.0f) * 100.0f), 0);
            break;
        }
        case 15: // Daily Levels
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.daily01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.daily02", (int)std::floor((stat / 25.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.daily03", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.daily04", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.daily05", (int)std::floor((stat / 150.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.daily06", (int)std::floor((stat / 250.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.daily07", (int)std::floor((stat / 365.0f) * 100.0f), 0);
            break;
        }
        case 16: // Shadow Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardShadow01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardShadow02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardShadow03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardShadow04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardShadow05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 17: // Poison Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardPoison01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardPoison02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardPoison03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardPoison04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardPoison05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 18: // Fire Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardFire01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardFire02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardFire03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardFire04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardFire05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 19: // Ice Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardIce01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardIce02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardIce03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardIce04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardIce05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 20: // Lava Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardLava01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLava02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLava03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLava04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLava05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 23: // Earth Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardEarth01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardEarth02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardEarth03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardEarth04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardEarth05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 24: // Blood Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardBlood01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardBlood02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardBlood03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardBlood04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardBlood05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 25: // Metal Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardMetal01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardMetal02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardMetal03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardMetal04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardMetal05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 26: // Light Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardLight01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLight02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLight03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLight04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardLight05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 27: // Soul Shards
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.shardSoul01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardSoul02", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardSoul03", (int)std::floor((stat / 35.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardSoul04", (int)std::floor((stat / 65.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.shardSoul05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            break;
        }
        case 28: // Moons
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.moons01", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons02", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons03", (int)std::floor((stat / 300.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons04", (int)std::floor((stat / 400.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons05", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons06", (int)std::floor((stat / 600.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons07", (int)std::floor((stat / 700.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons08", (int)std::floor((stat / 800.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons09", (int)std::floor((stat / 900.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons10", (int)std::floor((stat / 1000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons11", (int)std::floor((stat / 1500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons12", (int)std::floor((stat / 2000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons13", (int)std::floor((stat / 2500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons14", (int)std::floor((stat / 3000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons15", (int)std::floor((stat / 3500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons16", (int)std::floor((stat / 4000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons17", (int)std::floor((stat / 4500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons18", (int)std::floor((stat / 5000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons19", (int)std::floor((stat / 5500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons20", (int)std::floor((stat / 6000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons21", (int)std::floor((stat / 6500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons22", (int)std::floor((stat / 7000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons23", (int)std::floor((stat / 7500.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons24", (int)std::floor((stat / 8000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons25", (int)std::floor((stat / 9000.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.moons26", (int)std::floor((stat / 10000.0f) * 100.0f), 0);
            break;
        }
        case 40: // Gauntlets
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.gauntlets01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.gauntlets02", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.gauntlets03", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.gauntlets04", (int)std::floor((stat / 15.0f) * 100.0f), 0);
            break;
        }
        case 41: // Lists
        {
            int stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            if (stat > 0)
                gm->reportAchievementWithID("geometry.ach.lists01", 100, 0);
            
            gm->reportAchievementWithID("geometry.ach.lists02", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.lists03", (int)std::floor((stat / 25.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.lists04", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.lists05", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.lists06", (int)std::floor((stat / 150.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.lists07", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            break;
        }
        case 42: // Insane
        {
            float stat = getStat(statKey);
            GameManager* gm = GameManager::sharedState();
            
            gm->reportAchievementWithID("geometry.ach.insane01", (int)std::floor((stat / 5.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane02", (int)std::floor((stat / 10.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane03", (int)std::floor((stat / 25.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane04", (int)std::floor((stat / 50.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane05", (int)std::floor((stat / 75.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane06", (int)std::floor((stat / 100.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane07", (int)std::floor((stat / 200.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane08", (int)std::floor((stat / 300.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane09", (int)std::floor((stat / 400.0f) * 100.0f), 0);
            gm->reportAchievementWithID("geometry.ach.insane10", (int)std::floor((stat / 500.0f) * 100.0f), 0);
            break;
        }
        default:
            break;
    }
}

void GameStatsManager::resetSpecialStatAchievements()
{
    const char* achievementPrefixes[] = {
        "geometry.ach.diamonds",
        "geometry.ach.shardFire",
        "geometry.ach.shardIce",
        "geometry.ach.shardPoison",
        "geometry.ach.shardShadow",
        "geometry.ach.shardLava",
        "geometry.ach.shardBonus",
        "geometry.ach.shardEarth",
        "geometry.ach.shardBlood",
        "geometry.ach.shardMetal",
        "geometry.ach.shardLight",
        "geometry.ach.shardSoul",
        "geometry.ach.shardBonusB"
    };
    
    int maxCounts[] = {18, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
    
    for (int i = 0; i < 13; i++)
    {
        for (int j = 1; j <= maxCounts[i]; j++)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%s%02d", achievementPrefixes[i], j);
            GameManager::sharedState()->resetAchievement(str->getCString());
        }
    }
}

void GameStatsManager::verifyPathAchievements()
{
    for (int path = 30; path <= 39; path++)
    {
        int pathStat = getStatFromKey((StatKey)path);
        int threshold = 100;
        
        for (int level = 1; level <= 10; level++)
        {
            if (pathStat < threshold)
            {
                cocos2d::CCString* str = cocos2d::CCString::createWithFormat("geometry.ach.path%02d.%02d", path - 29, level);
                GameManager::sharedState()->resetAchievement(str->getCString());
            }
            threshold += 100;
        }
    }
}

void GameStatsManager::verifyUserCoins()
{
    int stat = getStat("12");
    int coinCount = m_verifiedUserCoins->count();
    
    if (stat > coinCount)
    {
        recountUserCoins(false);
        return;
    }
    
    int threshold = 10;
    for (int i = 2; i <= 22; i++)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("geometry.ach.usercoins%02d", i);
        if (threshold <= stat)
        {
            if (AchievementManager::sharedState()->isAchievementEarned(str->getCString()))
            {
                recountUserCoins(true);
                return;
            }
        }
        threshold += 10;
    }
}

void GameStatsManager::recountUserCoins(bool force)
{
    int stat = getStat("12");
    int coinCount = m_verifiedUserCoins->count();
    
    if (stat > coinCount || force)
    {
        for (int i = 1; i <= 21; i++)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("geometry.ach.usercoins%02d", i);
            GameManager::sharedState()->resetAchievement(str->getCString());
        }
        
        if (coinCount <= 9)
        {
            const char* secretAchievements[] = {
                "geometry.ach.secret05", "geometry.ach.secret06", "geometry.ach.secret07",
                "geometry.ach.secret08", "geometry.ach.secret09", "geometry.ach.secret10",
                "geometry.ach.secret13", "geometry.ach.secret14", "geometry.ach.secret15",
                "geometry.ach.secret16", "geometry.ach.secret17"
            };
            
            for (const char* ach : secretAchievements)
            {
                GameManager::sharedState()->resetAchievement(ach);
            }
        }
        
        GameManager::sharedState()->checkUsedIcons();
        bool oldDontNotify = AchievementManager::sharedState()->m_dontNotify;
        AchievementManager::sharedState()->m_dontNotify = true;
        setStat("12", coinCount);
        AchievementManager::sharedState()->m_dontNotify = oldDontNotify;
    }
}

void GameStatsManager::recountSpecialStats()
{
    int totalCurrency = getTotalCollectedCurrency();
    
    if (totalCurrency != getStat("22"))
        setStat("22", totalCurrency);
    
    int totalDiamonds = getTotalCollectedDiamonds();
    int bonusKeys = (int)(totalCurrency / 500.0f);
    m_bonusKey.value = bonusKeys;
    m_bonusKey.seed = rand() % 1000;
    m_bonusKey.random = bonusKeys + m_bonusKey.seed;
    
    int shardCounts[12] = {0}; // Fire, Ice, Poison, Shadow, Lava, Earth, Blood, Metal, Light, Soul, Bonus, Gold
    
    // Count shards from all chests
    for (int chestType = 0; chestType < 3; chestType++)
    {
        cocos2d::CCDictionary* chestDict = nullptr;
        switch (chestType)
        {
            case 0: chestDict = m_dailyChests; break;
            case 1: chestDict = m_weeklyChest; break;
            case 2: chestDict = m_miscChests; break;
        }
        
        if (chestDict)
        {
            cocos2d::CCDictElement* element = chestDict->m_pElements;
            while (element)
            {
                GJRewardItem* item = (GJRewardItem*)element->m_pObject;
                totalCurrency += GJRewardItem::getRewardCount(item, (SpecialRewardItem)6); // Orbs
                totalDiamonds += GJRewardItem::getRewardCount(item, (SpecialRewardItem)8);
                shardCounts[0] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)1); // Fire
                shardCounts[1] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)2); // Ice
                shardCounts[2] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)3); // Poison
                shardCounts[3] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)4); // Shadow
                shardCounts[4] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)5); // Lava
                shardCounts[5] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)10); // Earth
                shardCounts[6] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)11); // Blood
                shardCounts[7] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)12); // Metal
                shardCounts[8] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)13); // Light
                shardCounts[9] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)14); // Soul
                shardCounts[10] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)6); // Bonus Key
                shardCounts[11] += GJRewardItem::getRewardCount(item, (SpecialRewardItem)15); // Gold Key
                element = (cocos2d::CCDictElement*)element->hh.next;
            }
        }
    }
    
    // Count video ad rewards
    if (m_worldAdvertChests)
    {
        cocos2d::CCDictElement* element = m_worldAdvertChests->m_pElements;
        while (element)
        {
            cocos2d::CCString* str = (cocos2d::CCString*)element->m_pObject;
            int orbs = str->intValue();
            if (orbs > 200) orbs = 200;
            totalCurrency += orbs;
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }
    
    // Process treasure room chests
    if (m_treasureRoomChests)
    {
        cocos2d::CCArray* keys = m_treasureRoomChests->allKeys();
        for (int i = 0; i < keys->count(); i++)
        {
            cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
            int chestID = key->intValue();
            int cost = keyCostForSecretChest(chestID);
            
            // Try to unlock with gold keys first
            if (shardCounts[11] >= cost && chestID > 6000)
            {
                shardCounts[11] -= cost;
                // Process chest rewards
                GJRewardItem* item = (GJRewardItem*)m_treasureRoomChests->objectForKey(key->getCString());
                if (item)
                {
                    cocos2d::CCArray* objects = item->m_rewardObjects;
                    for (int j = 0; j < objects->count(); j++)
                    {
                        cocos2d::CCObject* obj = objects->objectAtIndex(j);
                        switch ((int)obj[1].__vftable)
                        {
                            case 1: shardCounts[0] += obj[1].m_nLuaID; break;
                            case 2: shardCounts[1] += obj[1].m_nLuaID; break;
                            case 3: shardCounts[2] += obj[1].m_nLuaID; break;
                            case 4: shardCounts[3] += obj[1].m_nLuaID; break;
                            case 5: shardCounts[4] += obj[1].m_nLuaID; break;
                            case 7: totalCurrency += obj[1].m_nLuaID; break;
                            case 8: totalDiamonds += obj[1].m_nLuaID; break;
                            case 10: shardCounts[5] += obj[1].m_nLuaID; break;
                            case 11: shardCounts[6] += obj[1].m_nLuaID; break;
                            case 12: shardCounts[7] += obj[1].m_nLuaID; break;
                            case 13: shardCounts[8] += obj[1].m_nLuaID; break;
                            case 14: shardCounts[9] += obj[1].m_nLuaID; break;
                        }
                    }
                }
            }
            else if (shardCounts[10] >= cost && chestID > 6000)
            {
                shardCounts[10] -= cost;
                // Process chest rewards similarly
                GJRewardItem* item = (GJRewardItem*)m_treasureRoomChests->objectForKey(key->getCString());
                if (item) { /* process rewards */ }
            }
            else
            {
                m_treasureRoomChests->removeObjectForKey(key->getCString());
            }
        }
    }
    
    // Verify purchased items
    if (m_purchasedItems)
    {
        cocos2d::CCArray* keys = m_purchasedItems->allKeys();
        for (int i = 0; i < keys->count(); i++)
        {
            cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
            int index = key->intValue();
            cocos2d::CCString* priceStr = (cocos2d::CCString*)m_purchasedItems->valueForKey(key->getCString());
            int price = priceStr ? priceStr->intValue() : 0;
            
            GJStoreItem* item = getStoreItem(index);
            if (item)
                price = item->m_price.random - item->m_price.seed;
            
            if (shopTypeForItemID(index) == ShopType::Diamond)
            {
                if (price <= totalDiamonds)
                    totalDiamonds -= price;
                else
                    m_purchasedItems->removeObjectForKey(key->getCString());
            }
            else
            {
                if (price <= totalCurrency)
                    totalCurrency -= price;
                else
                    m_purchasedItems->removeObjectForKey(key->getCString());
            }
        }
    }
    
    // Update all stats
    bool oldDontNotify = AchievementManager::sharedState()->m_dontNotify;
    bool oldSkip = m_skipIncrementChallenge;
    AchievementManager::sharedState()->m_dontNotify = true;
    m_skipIncrementChallenge = true;
    
    resetSpecialStatAchievements();
    
    setStat("14", totalCurrency);
    setStat("13", totalDiamonds);
    setStat("29", totalDiamonds);
    setStat("21", shardCounts[10]);
    setStat("43", shardCounts[11]);
    setStat("18", shardCounts[0]); // Fire
    setStat("19", shardCounts[1]); // Ice
    setStat("17", shardCounts[2]); // Poison
    setStat("16", shardCounts[3]); // Shadow
    setStat("20", shardCounts[4]); // Lava
    setStat("23", shardCounts[5]); // Earth
    setStat("24", shardCounts[6]); // Blood
    setStat("25", shardCounts[7]); // Metal
    setStat("26", shardCounts[8]); // Light
    setStat("27", shardCounts[9]); // Soul
    
    // Path achievements
    for (int path = 6; path <= 15; path++)
    {
        if (isItemUnlocked(UnlockType::GJItem, path))
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("geometry.ach.path%02d.00", path - 5);
            GameManager::sharedState()->reportAchievementWithID(str->getCString(), 100, 0);
        }
    }
    
    // Gauntlet count
    int gauntletCount = 0;
    for (int i = 1; i <= 60; i++)
    {
        if (isGauntletChestUnlocked(i))
            gauntletCount++;
    }
    setStat("40", gauntletCount);
    setStat("41", m_completedLists->count());
    
    GameManager::sharedState()->checkUsedIcons();
    AchievementManager::sharedState()->m_dontNotify = oldDontNotify;
    m_skipIncrementChallenge = oldSkip;
}

void GameStatsManager::checkCoinAchievement(GJGameLevel* level)
{
    if (level->m_levelType != LevelType::Main)
        return;
    
    int levelID = level->m_levelID.random - level->m_levelID.seed;
    int coins = getCollectedCoinsForLevel(level);
    int percent = (int)std::floor((coins / 3.0f) * 100.0f);
    const char* achievementKey = nullptr;
    
    switch (levelID)
    {
        case 14:
            achievementKey = "geometry.ach.demoncoin01";
            break;
        case 18:
            achievementKey = "geometry.ach.demoncoin02";
            break;
        case 20:
            achievementKey = "geometry.ach.demoncoin03";
            break;
        case 5001:
            achievementKey = "geometry.ach.tower01Coin";
            percent = (int)std::floor((coins / 3.0f) * 100.0f);
            break;
        case 5002:
            achievementKey = "geometry.ach.tower02Coin";
            percent = (int)std::floor((coins / 3.0f) * 100.0f);
            break;
        case 5003:
            achievementKey = "geometry.ach.tower03Coin";
            percent = (int)std::floor((coins / 3.0f) * 100.0f);
            break;
        case 5004:
            achievementKey = "geometry.ach.tower04Coin";
            percent = (int)std::floor((coins / 3.0f) * 100.0f);
            break;
        default:
            return;
    }
    
    if (achievementKey)
        GameManager::sharedState()->reportAchievementWithID(achievementKey, percent, 0);
}

void GameStatsManager::checkCoinsForLevel(GJGameLevel* level)
{
    if (level->m_coins <= 0)
        return;
    
    if (level->m_coinsVerified.random - level->m_coinsVerified.seed != 1)
        return;
    
    for (int i = 1; i <= 3; i++)
    {
        const char* coinKey = GJGameLevel::getCoinKey(level, i);
        if (hasPendingUserCoin(coinKey) && !hasUserCoin(coinKey))
        {
            storeUserCoin(coinKey);
            incrementStat("12");
        }
        m_pendingUserCoins->removeObjectForKey(coinKey);
    }
}

void GameStatsManager::awardOldSpecialStats()
{
    std::unordered_set<int> levelList;
    LevelTools::getLevelList(&levelList);
    
    for (int levelID : levelList)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(levelID, true);
        if (!level) continue;
        
        int normalPercent = GJGameLevel::getNormalPercent(level);
        int currentPercent = level->m_orbCompletion.random - level->m_orbCompletion.seed;
        
        if (normalPercent > currentPercent)
        {
            level->m_orbCompletion.value = normalPercent;
            int seed = rand() % 1000;
            level->m_orbCompletion.seed = seed;
            level->m_orbCompletion.random = normalPercent + seed;
        }
        
        awardCurrencyForLevel(level);
    }
}

void GameStatsManager::completedChallenge(GJChallengeItem* item)
{
    if (!item->m_canClaim || hasCompletedChallenge(item))
        return;
    
    int reward = item->m_reward.random - item->m_reward.seed;
    std::string key = getChallengeKey(item);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", reward);
    m_challengeDiamonds->setObject(str, key.c_str());
    incrementStat("13", reward);
}

GJRewardItem* GameStatsManager::completedDailyLevel(GJGameLevel* level)
{
    if (!level) return nullptr;
    
    int dailyID = level->m_dailyID.random - level->m_dailyID.seed;
    int stars = level->m_stars.random - level->m_stars.seed;
    int orbCompletion = level->m_orbCompletion.random - level->m_orbCompletion.seed;
    
    if (dailyID <= 0 || stars <= 0 || orbCompletion <= 99 || hasCompletedDailyLevel(dailyID))
        return nullptr;
    
    int bonusDiamonds = getBonusDiamonds(stars);
    int dailyIDValue = dailyID;
    
    if (dailyIDValue <= 99999)
    {
        std::string key = getDailyLevelKey(dailyID);
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", bonusDiamonds);
        m_challengeDiamonds->setObject(str, key.c_str());
        incrementStat("13", bonusDiamonds);
        return nullptr;
    }
    else if (dailyIDValue > 200000)
    {
        // Event level
        std::string key = getDailyLevelKey(dailyID);
        GJRewardItem* item = (GJRewardItem*)m_eventChest->objectForKey(key.c_str());
        if (item)
        {
            std::string chestKey = getEventRewardKey(dailyID);
            storeOnlineChest(chestKey, item);
            return unlockOnlineChest(chestKey);
        }
        return nullptr;
    }
    else
    {
        // Weekly level
        cocos2d::CCArray* rewards = cocos2d::CCArray::create();
        rewards->addObject(GJRewardObject::create((GJRewardObject*)8, bonusDiamonds, 0)); // Diamonds
        rewards->addObject(GJRewardObject::create((GJRewardObject*)7, 500, 0)); // Orbs
        rewards->addObject(GJRewardObject::create((GJRewardObject*)6, (SpecialRewardItem)1, 0)); // Fire Shard
        
        SpecialRewardItem randomShard = GJRewardItem::getRandomNonMaxShardType();
        if (randomShard != (SpecialRewardItem)0)
        {
            int shardCount = getStat(GJRewardItem::rewardItemToStat(randomShard).c_str());
            if (shardCount > 98)
                randomShard = (SpecialRewardItem)2; // Ice Shard
            rewards->addObject(GJRewardObject::create((GJRewardObject*)randomShard, (SpecialRewardItem)1, 0));
        }
        
        GJRewardItem* item = GJRewardItem::createWithObjects(GJRewardType::SmallTreasure, rewards);
        if (!item) return nullptr;
        
        std::string key = getDailyLevelKey(dailyID);
        if (!m_weeklyChest->objectForKey(key.c_str()))
        {
            registerRewardsFromItem(item);
            m_weeklyChest->setObject(item, key.c_str());
        }
        
        return item;
    }
}

bool GameStatsManager::claimListReward(GJLevelList* list)
{
    int completed = GJLevelList::completedLevels(list);
    int diamonds = list->m_diamonds;
    
    if (completed < list->m_levelsToClaim || list->m_levelsToClaim <= 0 || diamonds <= 0)
        return false;
    
    if (hasClaimedListReward(list))
        return false;
    
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", diamonds);
    std::string key = getListRewardKey(list);
    m_completedLists->setObject(str, key.c_str());
    incrementStat("13", diamonds);
    incrementStat("41");
    return true;
}

void GameStatsManager::collectReward(GJRewardType type, GJRewardItem* item)
{
    if (!item) return;
    if (hasRewardBeenCollected(type, item->m_chestID)) return;
    
    registerRewardsFromItem(item);
    std::string key = getRewardKey(type, item->m_chestID);
    m_dailyChests->setObject(item, key.c_str());
}

void GameStatsManager::collectVideoReward(int orbs)
{
    struct timespec ts;
    clock_gettime(0, &ts);
    double timestamp = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    
    cocos2d::CCString* timeStr = cocos2d::CCString::createWithFormat("%f", timestamp);
    cocos2d::CCString* orbsStr = cocos2d::CCString::createWithFormat("%i", orbs);
    m_worldAdvertChests->setObject(orbsStr, timeStr->getCString());
    
    m_skipIncrementChallenge = true;
    incrementStat("14", orbs);
    m_skipIncrementChallenge = false;
}

bool GameStatsManager::hasRewardBeenCollected(GJRewardType type, int id)
{
    std::string key = getRewardKey(type, id);
    return m_dailyChests->objectForKey(key.c_str()) != nullptr;
}

GJChallengeItem* GameStatsManager::getChallenge(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", id);
    return (GJChallengeItem*)m_activeChallenges->objectForKey(str->getCString());
}

GJChallengeItem* GameStatsManager::getQueuedChallenge(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", id);
    return (GJChallengeItem*)m_upcomingChallenges->objectForKey(str->getCString());
}

GJChallengeItem* GameStatsManager::getSecondaryQueuedChallenge(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", id + 100);
    return (GJChallengeItem*)m_upcomingChallenges->objectForKey(str->getCString());
}

void GameStatsManager::storeChallenge(int position, GJChallengeItem* challenge)
{
    challenge->m_position = position;
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", position);
    m_activeChallenges->setObject(challenge, str->getCString());
}

void GameStatsManager::storeQueuedChallenge(int position, GJChallengeItem* challenge)
{
    challenge->m_position = position;
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", position);
    m_upcomingChallenges->setObject(challenge, str->getCString());
}

void GameStatsManager::storeSecondaryQueuedChallenge(int position, GJChallengeItem* challenge)
{
    challenge->m_position = position;
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", position + 100);
    m_upcomingChallenges->setObject(challenge, str->getCString());
}

void GameStatsManager::removeChallenge(int position)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", position);
    m_activeChallenges->removeObjectForKey(str->getCString());
}

void GameStatsManager::removeQueuedChallenge(int position)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", position);
    m_upcomingChallenges->removeObjectForKey(str->getCString());
}

void GameStatsManager::removeQueuedSecondaryChallenge(int position)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", position + 100);
    m_upcomingChallenges->removeObjectForKey(str->getCString());
}

void GameStatsManager::processChallengeQueue(int position)
{
    GJChallengeItem* primary = getQueuedChallenge(position);
    GJChallengeItem* secondary = getSecondaryQueuedChallenge(position);
    
    removeChallenge(position);
    
    if (primary)
    {
        storeChallenge(position, primary);
        removeQueuedChallenge(position);
    }
    
    if (secondary)
    {
        storeQueuedChallenge(position, secondary);
        removeQueuedSecondaryChallenge(position);
    }
}

void GameStatsManager::storeChallengeTime(int remaining)
{
    struct timespec ts;
    clock_gettime(0, &ts);
    m_challengeTime = (double)remaining + (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

void GameStatsManager::registerRewardsFromItem(GJRewardItem* item)
{
    if (!item || !item->m_rewardObjects) return;
    
    m_skipIncrementChallenge = true;
    
    for (int i = 0; i < item->m_rewardObjects->count(); i++)
    {
        cocos2d::CCObject* obj = item->m_rewardObjects->objectAtIndex(i);
        switch ((int)obj[1].__vftable)
        {
            case 1: incrementStat("18", obj[1].m_nLuaID); break; // Fire Shard
            case 2: incrementStat("19", obj[1].m_nLuaID); break; // Ice Shard
            case 3: incrementStat("17", obj[1].m_nLuaID); break; // Poison Shard
            case 4: incrementStat("16", obj[1].m_nLuaID); break; // Shadow Shard
            case 5: incrementStat("20", obj[1].m_nLuaID); break; // Lava Shard
            case 6: incrementStat("21", obj[1].m_nLuaID); break; // Bonus Key
            case 7: incrementStat("14", obj[1].m_nLuaID); break; // Orbs
            case 8: incrementStat("13", obj[1].m_nLuaID); break; // Diamonds
            case 10: incrementStat("23", obj[1].m_nLuaID); break; // Earth Shard
            case 11: incrementStat("24", obj[1].m_nLuaID); break; // Blood Shard
            case 12: incrementStat("25", obj[1].m_nLuaID); break; // Metal Shard
            case 13: incrementStat("26", obj[1].m_nLuaID); break; // Light Shard
            case 14: incrementStat("27", obj[1].m_nLuaID); break; // Soul Shard
            case 15: incrementStat("43", obj[1].m_nLuaID); break; // Gold Key
        }
    }
    
    m_skipIncrementChallenge = false;
}

void GameStatsManager::preProcessReward(GJRewardItem* item)
{
    if (!item || !item->m_rewardObjects) return;
    
    int overflow = 0;
    
    for (int i = 0; i < item->m_rewardObjects->count(); i++)
    {
        cocos2d::CCObject* obj = item->m_rewardObjects->objectAtIndex(i);
        
        if (GJRewardItem::isShardType((SpecialRewardItem)obj[1].__vftable))
        {
            std::string statKey = GJRewardItem::rewardItemToStat((SpecialRewardItem)obj[1].__vftable);
            int currentCount = getStat(statKey.c_str());
            
            if (currentCount > 99)
            {
                int total = currentCount + obj[1].m_nLuaID;
                if (total > 100)
                {
                    obj[1].m_nLuaID = 100 - currentCount;
                    overflow += total - 100;
                    if (100 - currentCount <= 0)
                    {
                        item->m_rewardObjects->removeObjectAtIndex(i, true);
                        i--;
                        continue;
                    }
                }
            }
        }
    }
    
    if (overflow > 0)
    {
        GJRewardObject* orbObj = GJRewardItem::getRewardObjectForType(item, (SpecialRewardItem)6);
        if (orbObj)
            orbObj->m_total += overflow * 100;
    }
}

GJRewardItem* GameStatsManager::createReward(GJRewardType type, int id, std::string str)
{
    // Parse string like "7_500_8_100" where pairs are (type, amount)
    std::vector<std::string> tokens = GameToolbox::split(str, '_');
    cocos2d::CCArray* rewardObjects = cocos2d::CCArray::create();
    
    for (size_t i = 0; i + 1 < tokens.size(); i += 2)
    {
        int rewardType = atoi(tokens[i].c_str());
        int amount = atoi(tokens[i + 1].c_str());
        
        GJRewardObject* obj = nullptr;
        
        if (rewardType > 1000)
        {
            // Item unlock
            obj = GJRewardObject::createItemUnlock((UnlockType)(rewardType - 1000), amount);
        }
        else if (rewardType >= 1 && rewardType <= 14 && rewardType != 9)
        {
            // Special reward
            obj = GJRewardObject::create((GJRewardObject*)rewardType, (SpecialRewardItem)amount, 0);
        }
        
        if (obj)
            rewardObjects->addObject(obj);
    }
    
    if (rewardObjects->count() == 0)
        return nullptr;
    
    GJRewardItem* item = GJRewardItem::createWithObjects(type, rewardObjects);
    item->m_chestID = id;
    return item;
}

GJRewardItem* GameStatsManager::getRewardItem(GJRewardType type)
{
    return (GJRewardItem*)m_rewardItems->objectForKey(std::to_string((int)type).c_str());
}

GJRewardItem* GameStatsManager::getRewardForSecretChest(int id)
{
    return (GJRewardItem*)m_allTreasureRoomChests->objectForKey(std::to_string(id).c_str());
}

GJRewardItem* GameStatsManager::getRewardForSpecialChest(std::string key)
{
    return (GJRewardItem*)m_allSpecialChests->objectForKey(key.c_str());
}

void GameStatsManager::storeOnlineChest(std::string key, GJRewardItem* item)
{
    if (item)
        m_allSpecialChests->setObject(item, key.c_str());
}

void GameStatsManager::storeEventChest(int eventID, GJRewardItem* item)
{
    if (!item) return;
    std::string key = getEventRewardKey(eventID);
    m_eventChest->setObject(item, key.c_str());
}

GJRewardItem* GameStatsManager::unlockOnlineChest(std::string key)
{
    GJRewardItem* item = getRewardForSpecialChest(key);
    if (!item) return nullptr;
    
    GJRewardItem* result = unlockSpecialChest(key);
    
    if (result)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", 1);
        m_wraithChests->setObject(str, key.c_str());
        linkSpecialChestUnlocks(result, key);
    }
    
    return result;
}

GJRewardItem* GameStatsManager::unlockSecretChest(int id)
{
    if (isSecretChestUnlocked(id))
        return nullptr;
    
    GJRewardItem* item = getRewardForSecretChest(id);
    if (!item) return nullptr;
    
    int bonusKeys = getStat("21");
    int cost = keyCostForSecretChest(item->m_chestID);
    
    if (bonusKeys < cost)
        return nullptr;
    
    setStat("21", bonusKeys - cost);
    preProcessReward(item);
    registerRewardsFromItem(item);
    
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", id);
    m_treasureRoomChests->setObject(item, str->getCString());
    return item;
}

GJRewardItem* GameStatsManager::unlockGoldChest(int id)
{
    if (isSecretChestUnlocked(id))
        return nullptr;
    
    GJRewardItem* item = getRewardForSecretChest(id);
    if (!item) return nullptr;
    
    int goldKeys = getStat("43");
    if (goldKeys <= 0) return nullptr;
    
    setStat("43", goldKeys - 1);
    preProcessReward(item);
    registerRewardsFromItem(item);
    
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", id);
    m_treasureRoomChests->setObject(item, str->getCString());
    return item;
}

GJRewardItem* GameStatsManager::unlockSpecialChest(std::string key)
{
    if (isSpecialChestUnlocked(key))
        return nullptr;
    
    GJRewardItem* item = getRewardForSpecialChest(key);
    if (!item) return nullptr;
    
    preProcessReward(item);
    registerRewardsFromItem(item);
    m_miscChests->setObject(item, key.c_str());
    return item;
}

GJRewardItem* GameStatsManager::unlockGauntletChest(int id)
{
    std::string key = getGauntletRewardKey(id);
    GJRewardItem* item = unlockSpecialChest(key);
    
    if (item)
        incrementStat("40");
    
    return item;
}

GJRewardItem* GameStatsManager::unlockPathChest(int id)
{
    std::string key = getPathRewardKey(id);
    return unlockSpecialChest(key);
}

bool GameStatsManager::isSecretChestUnlocked(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", id);
    return m_treasureRoomChests->objectForKey(str->getCString()) != nullptr;
}

bool GameStatsManager::isSpecialChestUnlocked(std::string key)
{
    return m_miscChests->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::isSpecialChestLiteUnlockable(std::string key)
{
    return m_specialChestsLite && m_specialChestsLite->objectForKey(key.c_str()) != nullptr;
}

bool GameStatsManager::isGauntletChestUnlocked(int id)
{
    std::string key = getGauntletRewardKey(id);
    return isSpecialChestUnlocked(key);
}

bool GameStatsManager::isPathChestUnlocked(int path)
{
    std::string key = getPathRewardKey(path);
    return isSpecialChestUnlocked(key);
}

void GameStatsManager::resetSpecialChest(std::string key)
{
    m_miscChests->removeObjectForKey(key.c_str());
}

void GameStatsManager::removeErrorFromSpecialChests()
{
    processOnlineChests();
    
    cocos2d::CCArray* keys = m_miscChests->allKeys();
    for (int i = 0; i < keys->count(); i++)
    {
        cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
        if (!m_allSpecialChests->objectForKey(key->getCString()) &&
            !m_wraithChests->objectForKey(key->getCString()))
        {
            m_miscChests->removeObjectForKey(key->getCString());
        }
    }
}

void GameStatsManager::linkSpecialChestUnlocks(GJRewardItem* item, std::string key)
{
    if (!item) return;
    
    for (int i = 0; i < item->m_rewardObjects->count(); i++)
    {
        cocos2d::CCObject* obj = item->m_rewardObjects->objectAtIndex(i);
        if ((int)obj[1].__vftable == 9) // Item unlock
        {
            std::string itemKey = getItemKey(obj[1].m_uID, (int)obj[1].__vftable);
            m_allSpecialChestItems->setObject(cocos2d::CCString::create(key.c_str()), itemKey.c_str());
            m_createSpecialChestItemsMap[itemKey] = key;
        }
    }
}

void GameStatsManager::processOnlineChests()
{
    cocos2d::CCArray* keys = m_wraithChests->allKeys();
    for (int i = 0; i < keys->count(); i++)
    {
        cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
        GJRewardItem* item = (GJRewardItem*)m_miscChests->objectForKey(key->getCString());
        if (item)
            linkSpecialChestUnlocks(item, key->getCString());
    }
}

int GameStatsManager::countSecretChests(GJRewardType rewardType)
{
    if (rewardType == GJRewardType::SmallTreasure)
        return m_allTreasureRoomChests->count();
    
    int count = 0;
    cocos2d::CCDictElement* element = m_allTreasureRoomChests->m_pElements;
    while (element)
    {
        GJRewardItem* item = (GJRewardItem*)element->m_pObject;
        if (item && (GJRewardType)item->m_type == rewardType)
            count++;
        element = (cocos2d::CCDictElement*)element->hh.next;
    }
    return count;
}

int GameStatsManager::countUnlockedSecretChests(GJRewardType rewardType)
{
    if (rewardType == GJRewardType::SmallTreasure)
        return m_treasureRoomChests->count();
    
    int count = 0;
    cocos2d::CCDictElement* element = m_treasureRoomChests->m_pElements;
    while (element)
    {
        GJRewardItem* item = (GJRewardItem*)element->m_pObject;
        if (item && (GJRewardType)item->m_type == rewardType)
            count++;
        element = (cocos2d::CCDictElement*)element->hh.next;
    }
    return count;
}

int GameStatsManager::keyCostForSecretChest(int id)
{
    if (id <= 1000) return 1;
    if (id <= 2000) return 5;
    if (id <= 3000) return 10;
    if (id <= 4000) return 25;
    if (id <= 5000) return 50;
    if (id <= 6000) return 100;
    return 1;
}

int GameStatsManager::getNextGoldChestID()
{
    for (int id = 6001; id < 6021; id++)
    {
        if (!isSecretChestUnlocked(id))
            return id;
    }
    return 0;
}

bool GameStatsManager::isSecretCoinValid(std::string key)
{
    std::vector<std::string> parts = GameToolbox::split(key, '_');
    
    if (parts.size() == 2)
    {
        // Check if it's a valid secret coin key
        if (parts[0] == "secret04" || parts[0] == "secret06" || parts[0] == "secretB03")
            return true;
    }
    else if (parts.size() == 3)
    {
        int levelID = atoi(parts[0].c_str());
        int coinID = atoi(parts[1].c_str());
        return (levelID >= 5001 && levelID <= 5004) || (levelID >= 1 && levelID <= 22);
    }
    
    return false;
}

GJStoreItem* GameStatsManager::getStoreItem(int index)
{
    std::string key = std::to_string(index);
    return (GJStoreItem*)m_allStoreItems->objectForKey(key.c_str());
}

bool GameStatsManager::isStoreItemUnlocked(int index)
{
    std::string key = std::to_string(index);
    return m_purchasedItems->objectForKey(key.c_str()) != nullptr;
}

ShopType GameStatsManager::shopTypeForItemID(int index)
{
    GJStoreItem* item = getStoreItem(index);
    return item ? item->m_shopType : ShopType::Normal;
}

bool GameStatsManager::purchaseItem(int index)
{
    if (index <= 0) return false;
    if (isStoreItemUnlocked(index)) return false;
    
    GJStoreItem* item = getStoreItem(index);
    if (!item) return false;
    
    std::string currencyKey = GJStoreItem::getCurrencyKey(item);
    int currentCurrency = getStat(currencyKey.c_str());
    int price = item->m_price.random - item->m_price.seed;
    
    if (currentCurrency < price) return false;
    
    setStat(currencyKey.c_str(), currentCurrency - price);
    
    cocos2d::CCString* priceStr = cocos2d::CCString::createWithFormat("%i", price);
    cocos2d::CCString* keyStr = cocos2d::CCString::createWithFormat("%i", index);
    m_purchasedItems->setObject(priceStr, keyStr->getCString());
    
    return true;
}

bool GameStatsManager::isItemUnlocked(UnlockType type, int id)
{
    GJStoreItem* storeItem = getStoreItem(id);
    if (storeItem && isStoreItemUnlocked(storeItem->m_index.random - storeItem->m_index.seed))
        return true;
    
    int secretChestID = getSecretChestForItem(id, type);
    if (secretChestID > 0 && isSecretChestUnlocked(secretChestID))
        return true;
    
    cocos2d::CCString* specialKey = getSpecialChestKeyForItem(id, type);
    if (specialKey)
    {
        std::string key = specialKey->getCString();
        return isSpecialChestUnlocked(key);
    }
    
    return false;
}

bool GameStatsManager::isItemEnabled(UnlockType type, int id)
{
    if (!isItemUnlocked(type, id))
        return false;
    
    std::string key = getItemKey(id, (int)type);
    cocos2d::CCString* str = (cocos2d::CCString*)m_enabledItems->valueForKey(key.c_str());
    return str ? str->boolValue() : true;
}

int GameStatsManager::getItemUnlockState(int itemID, UnlockType unlockType)
{
    std::string key = getItemKey(itemID, (int)unlockType);
    GJUnlockableItem* item = (GJUnlockableItem*)m_unlockedItems->objectForKey(key.c_str());
    
    if (item)
        return item->getTag();
    
    // Check if it's a wraith icon
    auto it = m_wraithIcons.find({unlockType, itemID});
    if (it != m_wraithIcons.end())
        return 3;
    
    return 0;
}

int GameStatsManager::getItemUnlockStateLite(int id, UnlockType type)
{
    AchievementManager* am = AchievementManager::sharedState();
    std::string achievementKey = AchievementManager::achievementForUnlock(am, id, type);
    bool isAvailable = am->isAchievementAvailable(achievementKey.c_str());
    
    int state = getItemUnlockState(id, type);
    
    if (state == 1)
        return isAvailable ? 1 : 0;
    
    if (state > 1)
    {
        cocos2d::CCString* specialKey = getSpecialChestKeyForItem(id, type);
        if (specialKey)
        {
            std::string key = specialKey->getCString();
            if (isSpecialChestLiteUnlockable(key))
                return 3;
        }
    }
    
    return 0;
}

int GameStatsManager::getSecretChestForItem(int id, UnlockType type)
{
    std::string key = getItemKey(id, (int)type);
    cocos2d::CCString* str = (cocos2d::CCString*)m_allTreasureRoomChestItems->valueForKey(key.c_str());
    return str ? str->intValue() : 0;
}

cocos2d::CCString* GameStatsManager::getSpecialChestKeyForItem(int id, UnlockType type)
{
    std::string key = getItemKey(id, (int)type);
    return (cocos2d::CCString*)m_allSpecialChestItems->valueForKey(key.c_str());
}

void GameStatsManager::toggleEnableItem(UnlockType type, int id, bool enabled)
{
    std::string key = getItemKey(id, (int)type);
    
    if (enabled)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", 1);
        m_enabledItems->setObject(str, key.c_str());
    }
    else
    {
        m_enabledItems->removeObjectForKey(key.c_str());
    }
}

void GameStatsManager::unlockGauntlet(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", 1);
    std::string key = getGauntletRewardKey(id);
    m_unlockedGauntlets->setObject(str, key.c_str());
}

bool GameStatsManager::isGauntletUnlocked(int id)
{
    return true; // All gauntlets are unlocked by default
}

std::string GameStatsManager::getSpecialRewardDescription(std::string key, bool unused)
{
    auto it = m_specialRewardDescriptions.find(key);
    if (it != m_specialRewardDescriptions.end())
        return it->second;
    return "";
}

std::string GameStatsManager::getSpecialUnlockDescription(int id, UnlockType type, bool unused)
{
    std::string key = getItemKey(id, (int)type);
    auto it = m_createSpecialChestItemsMap.find(key);
    if (it != m_createSpecialChestItemsMap.end())
    {
        return getSpecialRewardDescription(it->second, unused);
    }
    return "";
}

void GameStatsManager::addSpecialRewardDescription(std::string key, std::string description)
{
    m_specialRewardDescriptions[key] = description;
}

std::string GameStatsManager::usernameForAccountID(int id)
{
    auto it = m_usernameForAccountID.find(id);
    if (it != m_usernameForAccountID.end())
        return it->second;
    return "";
}

int GameStatsManager::accountIDForIcon(int id, UnlockType type)
{
    auto it = m_accountIDForIcon.find({id, type});
    if (it != m_accountIDForIcon.end())
        return it->second;
    return 0;
}

void GameStatsManager::createStoreItems()
{
    if (m_allStoreItems) return;
    
    m_allStoreItems = cocos2d::CCDictionary::create();
    m_allStoreItems->retain();
    
    m_storeItems = cocos2d::CCDictionary::create();
    m_storeItems->retain();
    
    m_storeItemArray = cocos2d::CCArray::create();
    m_storeItemArray->retain();
    
    // Main shop items
    addStoreItem(34, 79, 1, 500, ShopType::Normal);
    addStoreItem(1, 77, 1, 1000, ShopType::Normal);
    addStoreItem(2, 86, 1, 1000, ShopType::Normal);
    addStoreItem(3, 73, 1, 1000, ShopType::Normal);
    addStoreItem(4, 102, 1, 1000, ShopType::Normal);
    addStoreItem(5, 107, 1, 1000, ShopType::Normal);
    addStoreItem(6, 27, 4, 2000, ShopType::Normal);
    addStoreItem(7, 25, 6, 2000, ShopType::Normal);
    addStoreItem(8, 23, 6, 2500, ShopType::Normal);
    addStoreItem(9, 20, 5, 1000, ShopType::Normal);
    addStoreItem(10, 19, 5, 1500, ShopType::Normal);
    addStoreItem(11, 21, 7, 500, ShopType::Normal);
    addStoreItem(12, 2, 9, 2000, ShopType::Normal);
    addStoreItem(13, 12, 8, 3000, ShopType::Normal);
    addStoreItem(14, 8, 11, 7000, ShopType::Normal);
    addStoreItem(15, 11, 11, 7000, ShopType::Normal);
    addStoreItem(16, 18, 7, 2000, ShopType::Secret);
    addStoreItem(17, 94, 1, 3000, ShopType::Secret);
    addStoreItem(18, 85, 1, 2500, ShopType::Secret);
    addStoreItem(19, 96, 1, 2000, ShopType::Secret);
    addStoreItem(20, 4, 12, 1000, ShopType::Secret);
    addStoreItem(21, 14, 8, 3000, ShopType::Secret);
    addStoreItem(22, 21, 6, 4000, ShopType::Secret);
    addStoreItem(23, 3, 9, 3000, ShopType::Secret);
    addStoreItem(24, 25, 5, 2000, ShopType::Secret);
    addStoreItem(25, 13, 11, 10000, ShopType::Secret);
    addStoreItem(26, 35, 2, 1000, ShopType::Normal);
    addStoreItem(27, 36, 2, 1000, ShopType::Normal);
    addStoreItem(28, 39, 2, 1000, ShopType::Secret);
    addStoreItem(29, 40, 2, 1000, ShopType::Secret);
    addStoreItem(30, 39, 3, 1000, ShopType::Normal);
    addStoreItem(32, 32, 3, 1000, ShopType::Secret);
    addStoreItem(33, 41, 3, 1000, ShopType::Secret);
    addStoreItem(35, 5, 10, 4000, ShopType::Normal);
    addStoreItem(36, 6, 10, 4000, ShopType::Normal);
    addStoreItem(37, 7, 10, 4000, ShopType::Secret);
    addStoreItem(38, 117, 1, 4000, ShopType::Community);
    addStoreItem(39, 118, 1, 2000, ShopType::Community);
    addStoreItem(40, 119, 1, 2000, ShopType::Community);
    addStoreItem(41, 120, 1, 4000, ShopType::Community);
    addStoreItem(42, 121, 1, 2000, ShopType::Community);
    addStoreItem(43, 122, 1, 2000, ShopType::Community);
    addStoreItem(44, 123, 1, 6000, ShopType::Community);
    addStoreItem(45, 124, 1, 8000, ShopType::Community);
    addStoreItem(46, 125, 1, 4000, ShopType::Community);
    addStoreItem(47, 126, 1, 2000, ShopType::Community);
    addStoreItem(48, 127, 1, 6000, ShopType::Community);
    addStoreItem(49, 128, 1, 8000, ShopType::Community);
    addStoreItem(50, 129, 1, 6000, ShopType::Community);
    addStoreItem(51, 130, 1, 4000, ShopType::Community);
    addStoreItem(52, 131, 1, 4000, ShopType::Community);
    addStoreItem(53, 132, 1, 4000, ShopType::Community);
    addStoreItem(54, 133, 1, 8000, ShopType::Community);
    addStoreItem(55, 134, 1, 6000, ShopType::Community);
    addStoreItem(56, 135, 1, 6000, ShopType::Community);
    addStoreItem(57, 30, 5, 4000, ShopType::Community);
    addStoreItem(58, 31, 5, 6000, ShopType::Community);
    addStoreItem(59, 32, 5, 6000, ShopType::Community);
    addStoreItem(60, 33, 5, 2000, ShopType::Community);
    addStoreItem(61, 34, 5, 4000, ShopType::Community);
    addStoreItem(62, 35, 5, 8000, ShopType::Community);
    addStoreItem(63, 36, 5, 6000, ShopType::Community);
    addStoreItem(64, 37, 5, 2000, ShopType::Community);
    addStoreItem(65, 38, 5, 2000, ShopType::Community);
    addStoreItem(66, 36, 4, 2000, ShopType::Community);
    addStoreItem(67, 37, 4, 2000, ShopType::Community);
    addStoreItem(68, 38, 4, 6000, ShopType::Community);
    addStoreItem(69, 39, 4, 2000, ShopType::Community);
    addStoreItem(70, 40, 4, 4000, ShopType::Community);
    addStoreItem(71, 41, 4, 4000, ShopType::Community);
    addStoreItem(72, 42, 4, 6000, ShopType::Community);
    addStoreItem(73, 43, 4, 2000, ShopType::Community);
    addStoreItem(74, 44, 4, 6000, ShopType::Community);
    addStoreItem(75, 45, 4, 8000, ShopType::Community);
    addStoreItem(76, 46, 4, 4000, ShopType::Community);
    addStoreItem(77, 47, 4, 6000, ShopType::Community);
    addStoreItem(78, 48, 4, 4000, ShopType::Community);
    addStoreItem(79, 29, 6, 6000, ShopType::Community);
    addStoreItem(80, 30, 6, 6000, ShopType::Community);
    addStoreItem(81, 31, 6, 4000, ShopType::Community);
    addStoreItem(82, 32, 6, 6000, ShopType::Community);
    addStoreItem(83, 33, 6, 4000, ShopType::Community);
    addStoreItem(84, 34, 6, 2000, ShopType::Community);
    addStoreItem(85, 35, 6, 2000, ShopType::Community);
    addStoreItem(86, 24, 7, 2000, ShopType::Community);
    addStoreItem(87, 25, 7, 4000, ShopType::Community);
    addStoreItem(88, 26, 7, 2000, ShopType::Community);
    addStoreItem(89, 27, 7, 2000, ShopType::Community);
    addStoreItem(90, 28, 7, 6000, ShopType::Community);
    addStoreItem(91, 29, 7, 6000, ShopType::Community);
    addStoreItem(92, 30, 7, 4000, ShopType::Community);
    addStoreItem(93, 31, 7, 6000, ShopType::Community);
    addStoreItem(94, 32, 7, 2000, ShopType::Community);
    addStoreItem(95, 33, 7, 6000, ShopType::Community);
    addStoreItem(96, 18, 8, 2000, ShopType::Community);
    addStoreItem(97, 19, 8, 6000, ShopType::Community);
    addStoreItem(98, 20, 8, 2000, ShopType::Community);
    addStoreItem(99, 21, 8, 4000, ShopType::Community);
    addStoreItem(100, 22, 8, 6000, ShopType::Community);
    addStoreItem(101, 23, 8, 2000, ShopType::Community);
    addStoreItem(102, 24, 8, 6000, ShopType::Community);
    addStoreItem(103, 25, 8, 4000, ShopType::Community);
    addStoreItem(104, 11, 9, 4000, ShopType::Community);
    addStoreItem(105, 12, 9, 6000, ShopType::Community);
    addStoreItem(106, 13, 9, 4000, ShopType::Community);
    addStoreItem(107, 14, 9, 8000, ShopType::Community);
    addStoreItem(108, 15, 9, 2000, ShopType::Community);
    addStoreItem(109, 16, 9, 6000, ShopType::Community);
    addStoreItem(111, 140, 1, 3000, ShopType::Normal);
    addStoreItem(112, 109, 1, 4000, ShopType::Normal);
    addStoreItem(113, 113, 1, 4000, ShopType::Normal);
    addStoreItem(114, 40, 5, 4000, ShopType::Normal);
    addStoreItem(115, 35, 7, 4000, ShopType::Normal);
    addStoreItem(116, 33, 2, 1000, ShopType::Normal);
    addStoreItem(117, 28, 3, 1000, ShopType::Normal);
    addStoreItem(118, 136, 1, 5000, ShopType::Secret);
    addStoreItem(119, 110, 1, 5000, ShopType::Secret);
    addStoreItem(120, 16, 11, 15000, ShopType::Secret);
    addStoreItem(121, 6, 12, 25000, ShopType::Paths);
    addStoreItem(122, 7, 12, 25000, ShopType::Paths);
    addStoreItem(123, 8, 12, 25000, ShopType::Paths);
    addStoreItem(124, 9, 12, 25000, ShopType::Paths);
    addStoreItem(125, 10, 12, 25000, ShopType::Paths);
    addStoreItem(126, 11, 12, 25000, ShopType::Paths);
    addStoreItem(127, 12, 12, 25000, ShopType::Paths);
    addStoreItem(128, 13, 12, 25000, ShopType::Paths);
    addStoreItem(129, 14, 12, 25000, ShopType::Paths);
    addStoreItem(130, 15, 12, 25000, ShopType::Paths);
    addStoreItem(131, 162, 1, 3000, ShopType::Normal);
    addStoreItem(132, 40, 6, 3500, ShopType::Normal);
    addStoreItem(133, 84, 2, 1500, ShopType::Normal);
    addStoreItem(134, 104, 3, 2000, ShopType::Normal);
    addStoreItem(135, 7, 13, 5000, ShopType::Normal);
    addStoreItem(136, 97, 2, 2500, ShopType::Normal);
    addStoreItem(137, 33, 8, 3500, ShopType::Normal);
    addStoreItem(138, 18, 9, 4000, ShopType::Normal);
    addStoreItem(139, 57, 5, 3000, ShopType::Normal);
    addStoreItem(140, 49, 3, 2500, ShopType::Normal);
    addStoreItem(141, 38, 7, 5000, ShopType::Normal);
    addStoreItem(142, 191, 1, 3000, ShopType::Normal);
    addStoreItem(143, 99, 2, 1500, ShopType::Normal);
    addStoreItem(144, 88, 3, 2000, ShopType::Normal);
    addStoreItem(145, 229, 1, 5000, ShopType::Secret);
    addStoreItem(146, 86, 3, 1500, ShopType::Secret);
    addStoreItem(147, 58, 4, 4000, ShopType::Secret);
    addStoreItem(148, 83, 6, 3000, ShopType::Secret);
    addStoreItem(149, 45, 8, 3500, ShopType::Secret);
    addStoreItem(150, 12, 13, 6000, ShopType::Secret);
    addStoreItem(151, 206, 1, 2000, ShopType::Secret);
    addStoreItem(152, 94, 2, 1000, ShopType::Secret);
    addStoreItem(153, 60, 5, 2500, ShopType::Secret);
    addStoreItem(154, 107, 5, 3000, ShopType::Secret);
    addStoreItem(155, 69, 9, 5000, ShopType::Secret);
    addStoreItem(156, 77, 4, 4000, ShopType::Secret);
    addStoreItem(157, 49, 2, 1000, ShopType::Secret);
    addStoreItem(158, 70, 3, 1000, ShopType::Secret);
    addStoreItem(159, 137, 4, 4000, ShopType::Secret);
    addStoreItem(160, 27, 13, 6000, ShopType::Secret);
    addStoreItem(161, 225, 1, 3000, ShopType::Secret);
    addStoreItem(162, 85, 3, 1500, ShopType::Secret);
    addStoreItem(163, 78, 7, 2500, ShopType::Secret);
    addStoreItem(164, 58, 9, 3500, ShopType::Secret);
    addStoreItem(165, 104, 6, 2500, ShopType::Secret);
    addStoreItem(166, 168, 1, 5000, ShopType::Mechanic);
    addStoreItem(167, 230, 1, 6000, ShopType::Mechanic);
    addStoreItem(168, 253, 1, 7000, ShopType::Mechanic);
    addStoreItem(169, 358, 1, 6000, ShopType::Mechanic);
    addStoreItem(170, 126, 4, 7000, ShopType::Mechanic);
    addStoreItem(171, 111, 4, 6000, ShopType::Mechanic);
    addStoreItem(172, 144, 4, 5000, ShopType::Mechanic);
    addStoreItem(173, 164, 4, 7000, ShopType::Mechanic);
    addStoreItem(174, 64, 5, 5000, ShopType::Mechanic);
    addStoreItem(175, 49, 5, 6000, ShopType::Mechanic);
    addStoreItem(176, 107, 6, 6000, ShopType::Mechanic);
    addStoreItem(177, 136, 6, 5000, ShopType::Mechanic);
    addStoreItem(178, 86, 6, 7000, ShopType::Mechanic);
    addStoreItem(179, 77, 7, 7000, ShopType::Mechanic);
    addStoreItem(180, 85, 7, 6000, ShopType::Mechanic);
    addStoreItem(181, 68, 7, 5000, ShopType::Mechanic);
    addStoreItem(182, 52, 8, 6000, ShopType::Mechanic);
    addStoreItem(183, 68, 8, 7000, ShopType::Mechanic);
    addStoreItem(184, 50, 9, 6000, ShopType::Mechanic);
    addStoreItem(185, 56, 9, 6000, ShopType::Mechanic);
    addStoreItem(186, 6, 13, 6000, ShopType::Mechanic);
    addStoreItem(187, 16, 12, 40000, ShopType::Mechanic);
    addStoreItem(188, 17, 12, 2000, ShopType::Diamond);
    addStoreItem(189, 181, 1, 300, ShopType::Diamond);
    addStoreItem(190, 186, 1, 500, ShopType::Diamond);
    addStoreItem(191, 196, 1, 400, ShopType::Diamond);
    addStoreItem(192, 203, 1, 600, ShopType::Diamond);
    addStoreItem(193, 250, 1, 300, ShopType::Diamond);
    addStoreItem(194, 263, 1, 600, ShopType::Diamond);
    addStoreItem(195, 314, 1, 400, ShopType::Diamond);
    addStoreItem(196, 421, 1, 500, ShopType::Diamond);
    addStoreItem(197, 443, 1, 600, ShopType::Diamond);
    addStoreItem(198, 450, 1, 500, ShopType::Diamond);
    addStoreItem(199, 471, 1, 400, ShopType::Diamond);
    addStoreItem(200, 66, 4, 500, ShopType::Diamond);
    addStoreItem(201, 81, 4, 600, ShopType::Diamond);
    addStoreItem(202, 125, 4, 500, ShopType::Diamond);
    addStoreItem(203, 129, 4, 400, ShopType::Diamond);
    addStoreItem(204, 131, 4, 500, ShopType::Diamond);
    addStoreItem(205, 168, 4, 600, ShopType::Diamond);
    addStoreItem(206, 53, 5, 600, ShopType::Diamond);
    addStoreItem(207, 98, 5, 300, ShopType::Diamond);
    addStoreItem(208, 112, 5, 500, ShopType::Diamond);
    addStoreItem(209, 113, 5, 400, ShopType::Diamond);
    addStoreItem(210, 116, 5, 500, ShopType::Diamond);
    addStoreItem(211, 45, 6, 500, ShopType::Diamond);
    addStoreItem(212, 52, 6, 500, ShopType::Diamond);
    addStoreItem(213, 100, 6, 600, ShopType::Diamond);
    addStoreItem(214, 140, 6, 600, ShopType::Diamond);
    addStoreItem(215, 149, 6, 600, ShopType::Diamond);
    addStoreItem(216, 56, 7, 400, ShopType::Diamond);
    addStoreItem(217, 65, 7, 300, ShopType::Diamond);
    addStoreItem(218, 75, 7, 500, ShopType::Diamond);
    addStoreItem(219, 87, 7, 400, ShopType::Diamond);
    addStoreItem(220, 34, 8, 500, ShopType::Diamond);
    addStoreItem(221, 51, 8, 500, ShopType::Diamond);
    addStoreItem(222, 63, 8, 600, ShopType::Diamond);
    addStoreItem(223, 55, 5, 300, ShopType::Diamond);
    addStoreItem(224, 29, 9, 500, ShopType::Diamond);
    addStoreItem(225, 54, 9, 300, ShopType::Diamond);
    addStoreItem(226, 60, 9, 400, ShopType::Diamond);
    addStoreItem(227, 65, 9, 300, ShopType::Diamond);
    addStoreItem(228, 10, 13, 500, ShopType::Diamond);
    addStoreItem(229, 30, 13, 500, ShopType::Diamond);
    addStoreItem(230, 40, 13, 600, ShopType::Diamond);
    addStoreItem(231, 5, 14, 600, ShopType::Diamond);
    addStoreItem(232, 77, 2, 100, ShopType::Diamond);
    addStoreItem(233, 71, 2, 100, ShopType::Diamond);
    addStoreItem(234, 64, 2, 100, ShopType::Diamond);
    addStoreItem(235, 100, 2, 100, ShopType::Diamond);
    addStoreItem(236, 89, 3, 100, ShopType::Diamond);
    addStoreItem(237, 106, 3, 100, ShopType::Diamond);
    addStoreItem(238, 43, 3, 100, ShopType::Diamond);
    addStoreItem(239, 72, 3, 100, ShopType::Diamond);
    addStoreItem(240, 98, 2, 100, ShopType::Diamond);
    addStoreItem(241, 77, 3, 100, ShopType::Diamond);
    addStoreItem(242, 74, 2, 100, ShopType::Diamond);
    addStoreItem(243, 59, 3, 100, ShopType::Diamond);
    addStoreItem(244, 18, 12, 15000, ShopType::Mechanic);
    addStoreItem(245, 19, 12, 15000, ShopType::Mechanic);
    addStoreItem(246, 20, 12, 15000, ShopType::Mechanic);
    addStoreItem(247, 2, 15, 20000, ShopType::Mechanic);
    addStoreItem(248, 3, 15, 20000, ShopType::Mechanic);
    addStoreItem(249, 4, 15, 20000, ShopType::Mechanic);
    addStoreItem(250, 5, 15, 20000, ShopType::Mechanic);
    addStoreItem(251, 6, 15, 20000, ShopType::Mechanic);
    addStoreItem(252, 18, 11, 1500, ShopType::Diamond);
    addStoreItem(253, 19, 11, 15000, ShopType::Mechanic);
    addStoreItem(254, 465, 1, 400, ShopType::Diamond);
    addStoreItem(255, 62, 4, 500, ShopType::Diamond);
    addStoreItem(256, 63, 4, 300, ShopType::Diamond);
    addStoreItem(257, 81, 5, 500, ShopType::Diamond);
    addStoreItem(258, 78, 6, 300, ShopType::Diamond);
    addStoreItem(259, 80, 6, 400, ShopType::Diamond);
    addStoreItem(260, 80, 7, 300, ShopType::Diamond);
    addStoreItem(261, 82, 7, 500, ShopType::Diamond);
    addStoreItem(262, 49, 8, 500, ShopType::Diamond);
    addStoreItem(263, 62, 8, 300, ShopType::Diamond);
    addStoreItem(264, 64, 9, 500, ShopType::Diamond);
    addStoreItem(265, 66, 9, 500, ShopType::Diamond);
    addStoreItem(266, 41, 13, 400, ShopType::Diamond);
    addStoreItem(267, 42, 13, 500, ShopType::Diamond);
    addStoreItem(268, 463, 1, 400, ShopType::Diamond);
    addStoreItem(269, 464, 1, 500, ShopType::Diamond);
    addStoreItem(270, 185, 1, 6000, ShopType::Mechanic);
    addStoreItem(271, 190, 1, 5000, ShopType::Mechanic);
    addStoreItem(272, 65, 5, 5000, ShopType::Mechanic);
    addStoreItem(273, 54, 5, 5000, ShopType::Mechanic);
    addStoreItem(274, 95, 6, 7000, ShopType::Mechanic);
    addStoreItem(275, 141, 6, 500, ShopType::Diamond);
    addStoreItem(276, 22, 9, 400, ShopType::Diamond);
    addStoreItem(277, 290, 1, 400, ShopType::Diamond);
    addStoreItem(278, 360, 1, 6000, ShopType::Mechanic);
    addStoreItem(279, 80, 4, 6000, ShopType::Mechanic);
    addStoreItem(280, 87, 4, 400, ShopType::Diamond);
    addStoreItem(281, 456, 1, 400, ShopType::Diamond);
    addStoreItem(282, 480, 1, 5000, ShopType::Mechanic);
    addStoreItem(283, 132, 6, 6000, ShopType::Mechanic);
    addStoreItem(284, 478, 1, 400, ShopType::Diamond);
    addStoreItem(285, 474, 1, 300, ShopType::Diamond);
}

void GameStatsManager::createSecretChestItems()
{
    if (m_allTreasureRoomChestItems) return;
    
    m_allTreasureRoomChestItems = cocos2d::CCDictionary::create();
    m_allTreasureRoomChestItems->retain();
    
    m_allTreasureRoomChests = cocos2d::CCDictionary::create();
    m_allTreasureRoomChests->retain();
    
    createSecretChestRewards();
    
    if (!m_allTreasureRoomChests) return;
    
    cocos2d::CCDictElement* element = m_allTreasureRoomChests->m_pElements;
    while (element)
    {
        GJRewardItem* item = (GJRewardItem*)element->m_pObject;
        if (item && item->m_rewardObjects)
        {
            for (int i = 0; i < item->m_rewardObjects->count(); i++)
            {
                cocos2d::CCObject* obj = item->m_rewardObjects->objectAtIndex(i);
                if ((int)obj[1].__vftable == 9) // Item unlock
                {
                    std::string key = getItemKey(obj[1].m_uID, (int)obj[1].__vftable);
                    cocos2d::CCString* chestStr = cocos2d::CCString::createWithFormat("%i", (int)item->m_type);
                    m_allTreasureRoomChestItems->setObject(chestStr, key.c_str());
                }
            }
        }
        element = (cocos2d::CCDictElement*)element->hh.next;
    }
}

void GameStatsManager::createSecretChestRewards()
{
    // Small Treasure chests (IDs 1-400)
    cocos2d::CCArray* smallChests = cocos2d::CCArray::create();
    
    // Add various reward combinations for small chests
    for (int i = 0; i < 26; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 5, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 16; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 8; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 20, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 30, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 20; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 50, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 20; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 100, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 14; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 200, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 8; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 300, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 4; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 500, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 1000, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 10; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 100, 5, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 200, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 200, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    // Shard rewards
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::FireShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::FireShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::FireShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::IceShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::IceShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::IceShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::PoisonShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::PoisonShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::PoisonShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::ShadowShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::ShadowShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::ShadowShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::LavaShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::LavaShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::LavaShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    // Earth Shards
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::EarthShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::EarthShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::EarthShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    // Blood Shards
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::BloodShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::BloodShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::BloodShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    // Metal Shards
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::MetalShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::MetalShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::MetalShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    // Light Shards
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::LightShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::LightShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::LightShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    // Soul Shards
    for (int i = 0; i < 6; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::SoulShard, 1, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 3; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::SoulShard, 2, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::SoulShard, 3, (SpecialRewardItem)0, 0, 0, 0));
    
    // Item unlock rewards
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Death, 3)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Ufo, 21)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Ship, 16)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 97)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Ball, 19)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Wave, 26)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Robot, 7)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Spider, 10)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 40)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col1, 41)));
    
    // More shards and combinations
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::FireShard, 1, SpecialRewardItem::IceShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::FireShard, 1, SpecialRewardItem::PoisonShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::FireShard, 1, SpecialRewardItem::ShadowShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::FireShard, 1, SpecialRewardItem::LavaShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::IceShard, 1, SpecialRewardItem::PoisonShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::IceShard, 1, SpecialRewardItem::ShadowShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::IceShard, 1, SpecialRewardItem::LavaShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::PoisonShard, 1, SpecialRewardItem::ShadowShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::PoisonShard, 1, SpecialRewardItem::LavaShard, 1, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 0, SpecialRewardItem::ShadowShard, 1, SpecialRewardItem::LavaShard, 1, 0, 0));
    
    for (int i = 0; i < 18; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 10; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 20, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 30, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 30, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 0, 50, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    // More item unlocks
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 167)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 179)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 176)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 163)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 189)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 249)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 287)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 255)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 281)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 302)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 313)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 334)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 353)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 350)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 348)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 373)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 391)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 426)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 414)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col1, 101)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 58)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 61)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 63)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 66)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 67)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 69)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 76)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 78)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Col2, 80)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 245)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 278)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 282)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 285)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 294)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 333)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 335)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 376)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 428)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Cube, 420)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Ball, 118)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Wave, 83)));
    smallChests->addObject(GJRewardItem::createWithObject(GJRewardType::SmallTreasure, GJRewardObject::createItemUnlock(UnlockType::Wave, 69)));
    
    for (int i = 0; i < 20; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 50, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 20; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 100, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 16; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 200, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 10; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 200, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    for (int i = 0; i < 4; i++)
        smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 500, 20, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 200, 20, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 200, 20, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 500, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 500, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 500, 20, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 500, 20, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    smallChests->addObject(GJRewardItem::createSpecial(GJRewardType::SmallTreasure, 1000, 30, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0));
    
    // Assign chest IDs
    int chestID = 1;
    for (int i = 0; i < smallChests->count(); i++)
    {
        cocos2d::CCObject* obj = smallChests->objectAtIndex(i);
        obj[1].m_uID = chestID;
        m_allTreasureRoomChests->setObject(obj, std::to_string(chestID).c_str());
        chestID++;
    }
}

void GameStatsManager::createSpecialChestItems()
{
    if (m_allSpecialChestItems) return;
    
    m_allSpecialChestItems = cocos2d::CCDictionary::create();
    m_allSpecialChestItems->retain();
    
    m_allSpecialChests = cocos2d::CCDictionary::create();
    m_allSpecialChests->retain();
    
    m_specialChestsLite = cocos2d::CCDictionary::create();
    m_specialChestsLite->retain();
    
    // Gauntlet chests
    GJRewardItem* gauntletChest1 = GJRewardItem::createSpecial(GJRewardType::Small, 0, 10, SpecialRewardItem::FireShard, 3, (SpecialRewardItem)0, 0, 3, 30);
    std::string key1 = getGauntletRewardKey(1);
    m_allSpecialChests->setObject(gauntletChest1, key1.c_str());
    
    GJRewardItem* gauntletChest2 = GJRewardItem::createSpecial(GJRewardType::Small, 0, 10, SpecialRewardItem::IceShard, 3, (SpecialRewardItem)0, 0, 3, 35);
    std::string key2 = getGauntletRewardKey(2);
    m_allSpecialChests->setObject(gauntletChest2, key2.c_str());
    
    GJRewardItem* gauntletChest3 = GJRewardItem::createSpecial(GJRewardType::Small, 0, 10, SpecialRewardItem::PoisonShard, 3, (SpecialRewardItem)0, 0, 3, 34);
    std::string key3 = getGauntletRewardKey(3);
    m_allSpecialChests->setObject(gauntletChest3, key3.c_str());
    
    GJRewardItem* gauntletChest4 = GJRewardItem::createSpecial(GJRewardType::Small, 0, 10, SpecialRewardItem::ShadowShard, 3, (SpecialRewardItem)0, 0, 3, 36);
    std::string key4 = getGauntletRewardKey(4);
    m_allSpecialChests->setObject(gauntletChest4, key4.c_str());
    
    GJRewardItem* gauntletChest5 = GJRewardItem::createSpecial(GJRewardType::Small, 0, 10, SpecialRewardItem::LavaShard, 3, (SpecialRewardItem)0, 0, 3, 29);
    std::string key5 = getGauntletRewardKey(5);
    m_allSpecialChests->setObject(gauntletChest5, key5.c_str());
    
    GJRewardItem* gauntletChest6 = GJRewardItem::createSpecial(GJRewardType::Small, 500, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 3, 37);
    std::string key6 = getGauntletRewardKey(6);
    m_allSpecialChests->setObject(gauntletChest6, key6.c_str());
    
    // Large Gauntlet chests
    cocos2d::CCArray* largeGauntlet1 = cocos2d::CCArray::create();
    largeGauntlet1->addObject(GJRewardObject::createItemUnlock(UnlockType::Spider, 16));
    largeGauntlet1->addObject(GJRewardObject::createItemUnlock(UnlockType::GJItem, 1));
    largeGauntlet1->addObject(GJRewardObject::create((GJRewardObject*)8, SpecialRewardItem::EarthShard, 0));
    GJRewardItem* lg1 = GJRewardItem::createWithObjects(GJRewardType::Large, largeGauntlet1);
    std::string key7 = getGauntletRewardKey(7);
    m_allSpecialChests->setObject(lg1, key7.c_str());
    
    GJRewardItem* lg2 = GJRewardItem::createSpecial(GJRewardType::Large, 0, 10, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 9, 10);
    std::string key8 = getGauntletRewardKey(8);
    m_allSpecialChests->setObject(lg2, key8.c_str());
    
    cocos2d::CCArray* lg3Arr = cocos2d::CCArray::create();
    lg3Arr->addObject(GJRewardObject::createItemUnlock(UnlockType::Cube, 138));
    lg3Arr->addObject(GJRewardObject::create((GJRewardObject*)8, (SpecialRewardItem)50, 0));
    lg3Arr->addObject(GJRewardObject::create((GJRewardObject*)6, SpecialRewardItem::FireShard, 0));
    GJRewardItem* lg3 = GJRewardItem::createWithObjects(GJRewardType::Large, lg3Arr);
    std::string key9 = getGauntletRewardKey(9);
    m_allSpecialChests->setObject(lg3, key9.c_str());
    
    cocos2d::CCArray* lg4Arr = cocos2d::CCArray::create();
    lg4Arr->addObject(GJRewardObject::createItemUnlock(UnlockType::Cube, 139));
    lg4Arr->addObject(GJRewardObject::create((GJRewardObject*)8, (SpecialRewardItem)50, 0));
    lg4Arr->addObject(GJRewardObject::create((GJRewardObject*)6, SpecialRewardItem::FireShard, 0));
    GJRewardItem* lg4 = GJRewardItem::createWithObjects(GJRewardType::Large, lg4Arr);
    std::string key10 = getGauntletRewardKey(10);
    m_allSpecialChests->setObject(lg4, key10.c_str());
    
    // Continue for gauntlets 11-60...
    // (This would be a large loop, simplified here)
    for (int i = 11; i <= 60; i++)
    {
        cocos2d::CCArray* arr = cocos2d::CCArray::create();
        arr->addObject(GJRewardObject::createItemUnlock(UnlockType::Cube, 140 + i));
        arr->addObject(GJRewardObject::create((GJRewardObject*)8, (SpecialRewardItem)50, 0));
        arr->addObject(GJRewardObject::create((GJRewardObject*)6, SpecialRewardItem::FireShard, 0));
        GJRewardItem* item = GJRewardItem::createWithObjects(GJRewardType::Large, arr);
        std::string key = getGauntletRewardKey(i);
        m_allSpecialChests->setObject(item, key.c_str());
    }
    
    // Wraith icons
    std::vector<std::pair<UnlockType, int>> wraithIcons = {
        {UnlockType::Death, 13}, {UnlockType::Ship, 9}, {UnlockType::Death, 14},
        {UnlockType::Ship, 9}, {UnlockType::Death, 15}, {UnlockType::Cube, 112},
        {UnlockType::Cube, 114}, {UnlockType::Cube, 115}, {UnlockType::Cube, 116},
        {UnlockType::Cube, 137}, {UnlockType::Ufo, 39}, {UnlockType::Ufo, 42},
        {UnlockType::Ship, 34}, {UnlockType::Wave, 51}, {UnlockType::Spider, 26},
        {UnlockType::Robot, 17}, {UnlockType::Cube, 155}, {UnlockType::Cube, 180},
        {UnlockType::Cube, 171}, {UnlockType::Cube, 175}, {UnlockType::Cube, 177},
        {UnlockType::Cube, 273}, {UnlockType::Cube, 265}, {UnlockType::Cube, 322},
        {UnlockType::Cube, 291}, {UnlockType::Cube, 330}, {UnlockType::Ball, 97},
        {UnlockType::Ball, 53}, {UnlockType::Ball, 56}, {UnlockType::Ship, 42},
        {UnlockType::Ship, 76}, {UnlockType::Spider, 30}, {UnlockType::Ufo, 58},
        {UnlockType::Wave, 73}, {UnlockType::Cube, 289}, {UnlockType::Cube, 269}
    };
    
    for (auto& pair : wraithIcons)
        m_wraithIcons[pair] = 1;
    
    // Link all special chest unlocks
    cocos2d::CCArray* allKeys = m_allSpecialChests->allKeys();
    for (int i = 0; i < allKeys->count(); i++)
    {
        cocos2d::CCString* key = (cocos2d::CCString*)allKeys->objectAtIndex(i);
        GJRewardItem* item = (GJRewardItem*)m_allSpecialChests->objectForKey(key->getCString());
        if (item)
            linkSpecialChestUnlocks(item, key->getCString());
    }
}

void GameStatsManager::generateItemUnlockableData()
{
    if (m_unlockedItems) return;
    
    m_unlockedItems = cocos2d::CCDictionary::create();
    m_unlockedItems->retain();
    
    std::vector<std::pair<UnlockType, int>> unlockTypes = {
        {UnlockType::Cube, 485}, {UnlockType::Ship, 118}, {UnlockType::Ball, 149},
        {UnlockType::Ufo, 96}, {UnlockType::Wave, 68}, {UnlockType::Robot, 69},
        {UnlockType::Spider, 68}, {UnlockType::Death, 20}, {UnlockType::Col1, 107},
        {UnlockType::Col2, 107}, {UnlockType::GJItem, 43}, {UnlockType::Item1, 8},
        {UnlockType::Item2, 6}, {UnlockType::GJItem, 20}
    };
    
    for (auto& type : unlockTypes)
    {
        for (int id = 1; id <= (type.second == 107 ? 107 : type.second); id++)
        {
            GJUnlockableItem* item = nullptr;
            int flags = 0;
            
            // Check achievement
            AchievementManager* am = AchievementManager::sharedState();
            std::string achKey = am->achievementForUnlock(id, type.first);
            if (!achKey.empty())
            {
                item = GJUnlockableItem::create();
                item->setTag(1);
                flags++;
            }
            
            // Check store
            GJStoreItem* storeItem = getStoreItem(id);
            if (storeItem)
            {
                flags++;
                if (!item)
                {
                    item = GJUnlockableItem::create();
                    switch (storeItem->m_shopType)
                    {
                        case ShopType::Secret: item->setTag(5); break;
                        case ShopType::Community: item->setTag(6); break;
                        case ShopType::Mechanic: item->setTag(7); break;
                        case ShopType::Diamond: item->setTag(8); break;
                        default: item->setTag(4); break;
                    }
                }
            }
            
            // Check secret chest
            if (getSecretChestForItem(id, type.first) > 0)
            {
                flags++;
                if (!item)
                {
                    item = GJUnlockableItem::create();
                    item->setTag(2);
                }
            }
            
            // Check special chest
            if (getSpecialChestKeyForItem(id, type.first))
            {
                if (!item)
                {
                    item = GJUnlockableItem::create();
                    item->setTag(3);
                }
            }
            
            if (item)
            {
                std::string key = getItemKey(id, (int)type.first);
                m_unlockedItems->setObject(item, key.c_str());
            }
        }
    }
}

void GameStatsManager::setupIconCredits()
{
    // Clear existing data
    m_accountIDForIcon.clear();
    m_usernameForAccountID.clear();
    
    // Setup icon credits map
    // Each entry: (iconID, unlockType) -> accountID
    std::map<std::pair<int, UnlockType>, int> iconCredits = {
        {{117, UnlockType::Cube}, 3061610},
        {{118, UnlockType::Cube}, 5477818},
        {{119, UnlockType::Cube}, 1502678},
        {{120, UnlockType::Cube}, 5363076},
        {{121, UnlockType::Cube}, 2678947},
        {{122, UnlockType::Cube}, 4964707},
        {{123, UnlockType::Cube}, 3407871},
        {{124, UnlockType::Cube}, 4622865},
        {{125, UnlockType::Cube}, 141781},
        {{126, UnlockType::Cube}, 5458141},
        {{127, UnlockType::Cube}, 1055},
        {{128, UnlockType::Cube}, 1055},
        {{129, UnlockType::Cube}, 2626257},
        {{130, UnlockType::Cube}, 2838},
        {{131, UnlockType::Cube}, 2838},
        {{132, UnlockType::Cube}, 2838},
        {{133, UnlockType::Cube}, 252302},
        {{134, UnlockType::Cube}, 252302},
        {{135, UnlockType::Cube}, 119501}
    };
    
    m_accountIDForIcon = iconCredits;
    
    // Setup username map
    std::map<int, std::string> usernames = {
        {1502678, "RadleyMCT"},
        {1111063, "DSprint"},
        {617383, "RetroAdvance64"},
        {9431154, "NHJ"},
        {10470991, "Dibbj"},
        {12285856, "Axel996"},
        {2678947, "GhostPower13"},
        {5737229, "ZetkutGD"},
        {14023980, "TaMaLiToUwU"},
        {6046266, "redwn"},
        {2168397, "DaferSilver"},
        {5477818, "aruXD"},
        {5363076, "Djoxy"},
        {4964707, "hallymallybubu"},
        {3407871, "Ic3Fir3"},
        {4622865, "ItsJavi2504x"},
        {141781, "LaZye"},
        {5458141, "Last67706"},
        {1584303, "shacal"},
        {756232, "TriaScarlix"},
        {272294, "KingTony"},
        {3061610, "Aleck08"},
        {1055, "luisilloRS"},
        {2626257, "NeXuS Gamer"},
        {2838, "Rob Buck"},
        {252302, "Meikyu"},
        {119501, "Terron"},
        {5794524, "CygnusGD"},
        {820393, "Deraklich"},
        {7725202, "Edialex"},
        {4956388, "litexotic"},
        {3006276, "checho623"},
        {1093804, "AutoNick"},
        {8840229, "Ighby"},
        {5568086, "InsaneJohnson"},
        {5935068, "Yoshibros64"},
        {1788388, "Aleiz21"},
        {2102, "ML500"},
        {4861064, "Sakura"},
        {127035, "flash"},
        {1678677, "Querez"},
        {11334262, "airiLmao"},
        {8385005, "AlexanderX1ta"},
        {101834, "TriStorm"},
        {6615856, "Th3ih4g0GMD"},
        {13296775, "SupremeOrange"},
        {1275405, "Subwoofer"},
        {2816040, "Danke"},
        {1301742, "para"},
        {3828686, "KyNutZ"},
        {21113321, "cyanflower"},
        {92744, "maxgreenevsky"},
        {1449955, "Zigma"},
        {2317322, "rafer"},
        {538396, "xXLOCOXx"},
        {5434390, "ElGab"},
        {9238889, "Keoiki"},
        {1541587, "TamTixx"},
        {3768957, "DarkFireDrago"},
        {328441, "ScorchVx"},
        {5185839, "Seadawg02"},
        {1632845, "xVexis"},
        {3676830, "Splons"},
        {24316, "k3vo5ky"},
        {2829, "Mariolego320"},
        {893112, "Blitzer"},
        {992231, "SpaceSyrup"},
        {2565154, "Arachnus"},
        {6109127, "CyverMast"},
        {6667073, "lordwerthers"},
        {1290979, "Jntx231"},
        {4669292, "dracthedragon"},
        {2117373, "DaRealDazzer"},
        {137794, "Revanty"},
        {4119239, "Alex PG"},
        {155254, "Plazda"},
        {7395563, "RagnokGD"},
        {26994, "The Railgunner"},
        {6051650, "samuraivi13"},
        {135919, "haoN"},
        {332, "Gelt"},
        {-1, "CHRISTIAN"},
        {-2, "ITSSUNSET"},
        {-3, "VIRUS METAL"}
    };
    
    m_usernameForAccountID = usernames;
}

void GameStatsManager::resetPreSync()
{
    // Empty implementation - placeholder
}

void GameStatsManager::restorePostSync()
{
    // Empty implementation - placeholder
}

void GameStatsManager::tempClear()
{
    // Empty implementation - placeholder
}

void GameStatsManager::tryFixPathBug()
{
    if (!m_tryFixPathBug) return;
    
    bool oldDontNotify = AchievementManager::sharedState()->m_dontNotify;
    AchievementManager::sharedState()->m_dontNotify = true;
    m_tryFixPathBug = false;
    
    int totalStars = getStat("6") + getStat("28");
    int extraStars = 0;
    int maxExtra = 0;
    
    if (totalStars > 1000)
    {
        extraStars = totalStars - 1000;
        if (getStat("6") <= 30000)
            maxExtra = 5000;
        else if (getStat("6") <= 100000)
            maxExtra = 6000;
        else
            maxExtra = 7000;
        
        if (extraStars > maxExtra)
            extraStars = maxExtra;
    }
    
    // Store path stats
    std::vector<std::pair<int, int>> pathStats;
    for (int path = 30; path <= 39; path++)
    {
        int stat = getStatFromKey((StatKey)path);
        if (stat > 999)
            stat = 1000;
        pathStats.push_back({path, stat});
    }
    
    // Reset all path stats
    for (int path = 30; path <= 39; path++)
    {
        std::string key = GameToolbox::intToString(path);
        setStat(key.c_str(), 0);
    }
    
    // Reset path achievements
    for (int path = 6; path <= 15; path++)
    {
        for (int level = 1; level <= 10; level++)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("geometry.ach.path%02d.%02d", path - 5, level);
            GameManager::sharedState()->resetAchievement(str->getCString());
        }
    }
    
    // Restore path stats with extra stars
    for (auto& ps : pathStats)
    {
        int value = ps.second;
        if (extraStars > 0)
        {
            int add = std::min(extraStars, 1000);
            value += add;
            extraStars -= add;
        }
        
        if (value > 0)
        {
            std::string key = GameToolbox::intToString(ps.first);
            setStat(key.c_str(), value);
        }
        
        // Reset path chests if path is not complete
        if (ps.second <= 999)
        {
            std::string chestKey = getPathRewardKey(ps.first - 29);
            resetSpecialChest(chestKey);
        }
    }
    
    AchievementManager::sharedState()->m_dontNotify = oldDontNotify;
    m_pathBugFixed = true;
}

std::string GameStatsManager::getNextVideoAdReward()
{
    // Returns the next special chest key for video ad rewards
    static const char* chestKeys[] = {
        "0012", "0013", "0014", "0015", "0016", "0017", "0018", "0019", "0020", "0021"
    };
    
    for (const char* key : chestKeys)
    {
        if (!isSpecialChestUnlocked(key))
            return key;
    }
    
    return "";
}

void GameStatsManager::resetPreSync()
{
    // Empty implementation
}

void GameStatsManager::restorePostSync()
{
    // Empty implementation
}

void GameStatsManager::tempClear()
{
    // Empty implementation
}

void GameStatsManager::preSaveGameStats()
{
    bool oldDontNotify = AchievementManager::sharedState()->m_dontNotify;
    AchievementManager::sharedState()->m_dontNotify = true;
    
    // Save all stats to m_playerStats dictionary
    for (int i = 1; i <= 43; i++)
    {
        if (i >= 9 && i <= 11) continue; // Skip some stats
        if (i >= 29 && i <= 30) continue; // Skip some stats
        
        std::string key = GameToolbox::intToString(i);
        int value = getStat(key.c_str());
        
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", value);
        m_playerStats->setObject(str, key.c_str());
    }
    
    AchievementManager::sharedState()->m_dontNotify = oldDontNotify;
}

void GameStatsManager::postLoadGameStats()
{
    bool oldDontNotify = AchievementManager::sharedState()->m_dontNotify;
    AchievementManager::sharedState()->m_dontNotify = true;
    
    // Load all stats from m_playerStats dictionary
    for (int i = 1; i <= 43; i++)
    {
        if (i >= 9 && i <= 11) continue;
        if (i >= 29 && i <= 30) continue;
        
        std::string key = GameToolbox::intToString(i);
        cocos2d::CCString* str = (cocos2d::CCString*)m_playerStats->valueForKey(key.c_str());
        if (str)
        {
            int value = str->intValue();
            setStat(key.c_str(), value);
        }
    }
    
    AchievementManager::sharedState()->m_dontNotify = oldDontNotify;
}

void GameStatsManager::dataLoaded(DS_Dictionary* dict)
{
    int binaryVersion = DS_Dictionary::getIntegerForKey(dict, "binaryVersion");
    bool isVersion38 = binaryVersion == 38;
    
    if (!m_pathBugFixed)
    {
        m_pathBugFixed = DS_Dictionary::getBoolForKey(dict, "GS_29");
        if (!isVersion38 || !m_pathBugFixed)
        {
            m_tryFixPathBug = true;
        }
    }
    
    // Load player stats
    cocos2d::CCDictionary* statsDict = DS_Dictionary::getDictForKey(dict, "GS_value", 0);
    
    if (m_playerStats)
    {
        // Merge stats with existing
        cocos2d::CCArray* keys = statsDict->allKeys();
        for (int i = 0; i < keys->count(); i++)
        {
            cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
            int keyInt = key->intValue();
            
            // Skip certain keys that need special handling
            if (keyInt >= 30 && keyInt <= 39 && !isVersion38 && !m_tryFixPathBug)
                continue;
            
            cocos2d::CCString* newStr = (cocos2d::CCString*)statsDict->valueForKey(key->getCString());
            cocos2d::CCString* oldStr = (cocos2d::CCString*)m_playerStats->valueForKey(key->getCString());
            
            int newVal = newStr->intValue();
            int oldVal = oldStr ? oldStr->intValue() : 0;
            
            // For secret coins, take the max
            if (isSecretCoin(key->getCString()))
            {
                if (oldVal < newVal)
                {
                    m_playerStats->setObject(newStr, key->getCString());
                    if (!m_playerStats->objectForKey(key->getCString()))
                    {
                        m_playerStats->setObject(m_trueString, key->getCString());
                        incrementStat("8");
                    }
                }
            }
            else
            {
                // For other stats, take the max
                int finalVal = std::max(oldVal, newVal);
                if (finalVal != oldVal)
                {
                    cocos2d::CCString* finalStr = cocos2d::CCString::createWithFormat("%i", finalVal);
                    m_playerStats->setObject(finalStr, key->getCString());
                }
            }
        }
    }
    else if (statsDict)
    {
        statsDict->retain();
        m_playerStats = statsDict;
    }
    
    // Fix path bug if needed
    if (!m_pathBugFixed && !m_tryFixPathBug)
    {
        int totalPathStats = 0;
        for (int path = 30; path <= 39; path++)
        {
            int stat = getStatFromKey((StatKey)path);
            if (stat > 999)
                totalPathStats += stat;
            else
                totalPathStats += stat;
        }
        
        if (totalPathStats > 1000)
        {
            m_pathBugFixed = true;
        }
    }
    
    // Load verified user coins
    cocos2d::CCDictionary* verifiedCoins = DS_Dictionary::getDictForKey(dict, "GS_3", 0);
    if (verifiedCoins)
    {
        if (m_verifiedUserCoins)
        {
            // Merge coins
            cocos2d::CCArray* keys = verifiedCoins->allKeys();
            for (int i = 0; i < keys->count(); i++)
            {
                cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
                if (!m_verifiedUserCoins->objectForKey(key->getCString()))
                {
                    m_verifiedUserCoins->setObject(m_trueString, key->getCString());
                    incrementStat("12");
                }
            }
        }
        else
        {
            verifiedCoins->retain();
            m_verifiedUserCoins = verifiedCoins;
        }
    }
    
    // Load pending user coins
    cocos2d::CCDictionary* pendingCoins = DS_Dictionary::getDictForKey(dict, "GS_4", 0);
    if (pendingCoins)
    {
        if (m_pendingUserCoins)
        {
            GameToolbox::mergeDictsSaveLargestInt(m_pendingUserCoins, pendingCoins);
        }
        else
        {
            pendingCoins->retain();
            m_pendingUserCoins = pendingCoins;
        }
    }
    
    // Load completed levels
    cocos2d::CCDictionary* completed = DS_Dictionary::getDictForKey(dict, "GS_completed", 0);
    if (completed && !m_completedLevels)
    {
        completed->retain();
        m_completedLevels = completed;
    }
    
    // Load completed map packs
    cocos2d::CCDictionary* mapPacks = DS_Dictionary::getDictForKey(dict, "GS_5", 0);
    if (mapPacks)
    {
        if (m_completedMappacks)
            GameToolbox::mergeDictsSaveLargestInt(m_completedMappacks, mapPacks);
        else
        {
            mapPacks->retain();
            m_completedMappacks = mapPacks;
        }
    }
    
    // Load purchased items
    cocos2d::CCDictionary* purchased = DS_Dictionary::getDictForKey(dict, "GS_6", 0);
    if (purchased)
    {
        if (m_purchasedItems)
            GameToolbox::mergeDictsSaveLargestInt(m_purchasedItems, purchased);
        else
        {
            purchased->retain();
            m_purchasedItems = purchased;
        }
    }
    
    // Load currency scores
    auto loadCurrencyDict = [&](const char* key, cocos2d::CCDictionary*& target) {
        cocos2d::CCDictionary* dict = DS_Dictionary::getDictForKey(dict, key, 0);
        if (dict)
        {
            if (target)
                GameToolbox::mergeDictsSaveLargestInt(target, dict);
            else
            {
                dict->retain();
                target = dict;
            }
        }
    };
    
    loadCurrencyDict("GS_7", m_onlineCurrencyScores);
    loadCurrencyDict("GS_23", m_gauntletCurrencyScores);
    loadCurrencyDict("GS_10", m_mainCurrencyScores);
    loadCurrencyDict("GS_16", m_timelyCurrencyScores);
    loadCurrencyDict("GS_9", m_onlineStars);
    loadCurrencyDict("GS_17", m_timelyStars);
    loadCurrencyDict("GS_18", m_gauntletDiamondScores);
    loadCurrencyDict("GS_24", m_timelyDiamondScores);
    loadCurrencyDict("GS_8", m_unusedCurrencyAwardDict);
    
    // Load chest dictionaries
    auto loadChestDict = [&](const char* key, cocos2d::CCDictionary*& target, bool skipConflict = true) {
        cocos2d::CCDictionary* dict = DS_Dictionary::getDictForKey(dict, key, 0);
        if (dict)
        {
            if (target)
            {
                if (skipConflict)
                    GameToolbox::mergeDictsSkipConflict(target, dict);
                else
                    GameToolbox::mergeDictsSaveLargestInt(target, dict);
            }
            else
            {
                dict->retain();
                target = dict;
            }
        }
    };
    
    loadChestDict("GS_11", m_dailyChests);
    loadChestDict("GS_25", m_weeklyChest);
    loadChestDict("GS_31", m_eventChest);
    loadChestDict("GS_22", m_worldAdvertChests);
    loadChestDict("GS_19", m_treasureRoomChests);
    loadChestDict("GS_30", m_wraithChests);
    loadChestDict("GS_21", m_miscChests);
    loadChestDict("GS_28", m_enabledItems);
    loadChestDict("GS_32", m_unlockedGauntlets);
    
    // Load challenges
    cocos2d::CCDictionary* activeChallenges = DS_Dictionary::getDictForKey(dict, "GS_12", 0);
    if (activeChallenges)
    {
        if (m_activeChallenges)
        {
            // Update challenge progress
            cocos2d::CCArray* keys = activeChallenges->allKeys();
            for (int i = 0; i < keys->count(); i++)
            {
                cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
                GJChallengeItem* newChallenge = (GJChallengeItem*)activeChallenges->objectForKey(key->getCString());
                GJChallengeItem* oldChallenge = (GJChallengeItem*)m_activeChallenges->objectForKey(key->getCString());
                
                if (oldChallenge && !hasCompletedChallenge(oldChallenge))
                {
                    // Update progress if not completed
                    if (oldChallenge->m_progress < newChallenge->m_progress)
                    {
                        oldChallenge->m_progress = newChallenge->m_progress;
                        if (oldChallenge->m_canClaim != newChallenge->m_canClaim)
                            oldChallenge->m_canClaim = newChallenge->m_canClaim;
                    }
                }
                else if (!oldChallenge && !hasCompletedChallenge(newChallenge))
                {
                    m_activeChallenges->setObject(newChallenge, key->getCString());
                }
            }
        }
        else
        {
            activeChallenges->retain();
            m_activeChallenges = activeChallenges;
        }
    }
    
    // Load upcoming challenges
    cocos2d::CCDictionary* upcoming = DS_Dictionary::getDictForKey(dict, "GS_15", 0);
    if (upcoming && !m_upcomingChallenges)
    {
        upcoming->retain();
        m_upcomingChallenges = upcoming;
    }
    
    // Load challenge diamonds
    loadChestDict("GS_14", m_challengeDiamonds, false);
    loadChestDict("GS_27", m_completedLists, false);
    
    // Load bonus key
    int bonusKeyValue = DS_Dictionary::getIntegerForKey(dict, "GS_20");
    if (bonusKeyValue > m_bonusKey.random - m_bonusKey.seed)
    {
        m_bonusKey.value = bonusKeyValue;
        m_bonusKey.seed = rand() % 1000;
        m_bonusKey.random = bonusKeyValue + m_bonusKey.seed;
    }
    
    // Load active path
    StatKey activePath = (StatKey)DS_Dictionary::getIntegerForKey(dict, "GS_26");
    updateActivePath(activePath);
    
    removeErrorFromSpecialChests();
    postLoadGameStats();
}

void GameStatsManager::encodeDataTo(DS_Dictionary* dict)
{
    preSaveGameStats();
    
    DS_Dictionary::setDictForKey(dict, "GS_value", m_playerStats);
    DS_Dictionary::setDictForKey(dict, "GS_completed", m_completedLevels);
    DS_Dictionary::setDictForKey(dict, "GS_3", m_verifiedUserCoins);
    DS_Dictionary::setDictForKey(dict, "GS_4", m_pendingUserCoins);
    DS_Dictionary::setDictForKey(dict, "GS_5", m_completedMappacks);
    DS_Dictionary::setDictForKey(dict, "GS_6", m_purchasedItems);
    DS_Dictionary::setDictForKey(dict, "GS_7", m_onlineCurrencyScores);
    DS_Dictionary::setDictForKey(dict, "GS_23", m_gauntletCurrencyScores);
    DS_Dictionary::setDictForKey(dict, "GS_8", m_unusedCurrencyAwardDict);
    DS_Dictionary::setDictForKey(dict, "GS_9", m_onlineStars);
    DS_Dictionary::setDictForKey(dict, "GS_10", m_mainCurrencyScores);
    DS_Dictionary::setDictForKey(dict, "GS_16", m_timelyCurrencyScores);
    DS_Dictionary::setDictForKey(dict, "GS_17", m_timelyStars);
    DS_Dictionary::setDictForKey(dict, "GS_18", m_gauntletDiamondScores);
    DS_Dictionary::setDictForKey(dict, "GS_24", m_timelyDiamondScores);
    DS_Dictionary::setDictForKey(dict, "GS_11", m_dailyChests);
    DS_Dictionary::setDictForKey(dict, "GS_22", m_worldAdvertChests);
    DS_Dictionary::setDictForKey(dict, "GS_25", m_weeklyChest);
    DS_Dictionary::setDictForKey(dict, "GS_31", m_eventChest);
    DS_Dictionary::setDictForKey(dict, "GS_12", m_activeChallenges);
    DS_Dictionary::setDictForKey(dict, "GS_15", m_upcomingChallenges);
    DS_Dictionary::setDictForKey(dict, "GS_14", m_challengeDiamonds);
    DS_Dictionary::setDictForKey(dict, "GS_27", m_completedLists);
    DS_Dictionary::setDictForKey(dict, "GS_19", m_treasureRoomChests);
    DS_Dictionary::setDictForKey(dict, "GS_30", m_wraithChests);
    DS_Dictionary::setDictForKey(dict, "GS_21", m_miscChests);
    DS_Dictionary::setDictForKey(dict, "GS_28", m_enabledItems);
    DS_Dictionary::setDictForKey(dict, "GS_32", m_unlockedGauntlets);
    
    DS_Dictionary::setIntegerForKey(dict, "GS_20", m_bonusKey.random - m_bonusKey.seed);
    DS_Dictionary::setIntegerForKey(dict, "GS_26", (int)m_activePath);
    DS_Dictionary::setBoolForKey(dict, "GS_29", m_pathBugFixed);
}

void GameStatsManager::resetPreSync()
{
    // Empty - placeholder
}

void GameStatsManager::restorePostSync()
{
    // Empty - placeholder
}

void GameStatsManager::tempClear()
{
    // Empty - placeholder
}

void GameStatsManager::tryFixPathBug()
{
    if (!m_tryFixPathBug) return;
    
    bool oldDontNotify = AchievementManager::sharedState()->m_dontNotify;
    AchievementManager::sharedState()->m_dontNotify = true;
    m_tryFixPathBug = false;
    
    int totalStars = getStat("6") + getStat("28");
    int extraStars = 0;
    int maxExtra = 0;
    
    if (totalStars > 1000)
    {
        extraStars = totalStars - 1000;
        int onlineStars = getStat("6");
        if (onlineStars <= 30000)
            maxExtra = 5000;
        else if (onlineStars <= 100000)
            maxExtra = 6000;
        else
            maxExtra = 7000;
        
        if (extraStars > maxExtra)
            extraStars = maxExtra;
    }
    
    // Collect path stats
    std::vector<std::pair<int, int>> pathStats;
    for (int path = 30; path <= 39; path++)
    {
        int stat = getStatFromKey((StatKey)path);
        pathStats.push_back({path, stat});
    }
    
    // Reset all path stats
    for (int path = 30; path <= 39; path++)
    {
        std::string key = GameToolbox::intToString(path);
        setStat(key.c_str(), 0);
    }
    
    // Reset path achievements
    for (int path = 6; path <= 15; path++)
    {
        for (int level = 1; level <= 10; level++)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("geometry.ach.path%02d.%02d", path - 5, level);
            GameManager::sharedState()->resetAchievement(str->getCString());
        }
    }
    
    // Restore path stats with extra stars
    for (auto& ps : pathStats)
    {
        int value = ps.second;
        if (extraStars > 0)
        {
            int add = std::min(extraStars, 1000);
            value += add;
            extraStars -= add;
        }
        
        if (value > 0)
        {
            std::string key = GameToolbox::intToString(ps.first);
            setStat(key.c_str(), value);
        }
        
        // Reset path chests if path is not complete
        if (ps.second <= 999)
        {
            std::string chestKey = getPathRewardKey(ps.first - 29);
            resetSpecialChest(chestKey);
        }
    }
    
    AchievementManager::sharedState()->m_dontNotify = oldDontNotify;
    m_pathBugFixed = true;
}

std::string GameStatsManager::getNextVideoAdReward()
{
    static const char* chestKeys[] = {
        "0012", "0013", "0014", "0015", "0016", 
        "0017", "0018", "0019", "0020", "0021"
    };
    
    for (const char* key : chestKeys)
    {
        if (!isSpecialChestUnlocked(key))
            return key;
    }
    
    return "";
}