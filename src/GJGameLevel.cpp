// GJGameLevel.cpp
#include "GJGameLevel.h"
#include "DS_Dictionary.h"
#include "LevelTools.h"
#include "GameLevelManager.h"
#include "MusicDownloadManager.h"

bool GJGameLevel::areCoinsVerified() {
    int count = 0;
    if (m_firstCoinVerified.random - m_firstCoinVerified.seed == 1) {
        count++;
    }
    if (m_secondCoinVerified.random - m_secondCoinVerified.seed == 1) {
        count++;
    }
    if (m_thirdCoinVerified.random - m_thirdCoinVerified.seed == 1) {
        count++;
    }
    return count >= m_coins || m_coins <= 0;
}

bool GJGameLevel::canEncode() {
    return true;
}

void GJGameLevel::copyLevelInfo(GJGameLevel* source) {
    GameLevelManager* manager = GameLevelManager::sharedState();
    
    // Copy level name
    std::string nextName = manager->getNextLevelName();
    m_levelName = nextName;
    
    // Copy level string
    m_levelString = source->m_levelString;
    
    // Copy basic properties
    m_audioTrack = source->m_audioTrack;
    m_twoPlayerMode = source->m_twoPlayerMode;
    m_songID = source->m_songID;
    m_levelLength = source->m_levelLength;
    m_timestamp = source->m_timestamp;
    m_coins = source->m_coins;
    
    unverifyCoins();
    
    // Copy editor state
    m_lastBuildTab = source->m_lastBuildTab;
    m_lastEditorZoom = source->m_lastEditorZoom;
    m_lastBuildPage = source->m_lastBuildPage;
    m_lastCameraPos = source->m_lastCameraPos;
    m_lastBuildGroupID = source->m_lastBuildGroupID;
    
    m_workingTime = 0;
    m_workingTime2 = source->m_workingTime + source->m_workingTime2;
    
    // Copy camera and layer data
    m_savedCameraPositions = source->m_savedCameraPositions;
    m_previewLock = source->m_previewLock;
    m_lockedEditorLayers = source->m_lockedEditorLayers;
    
    // Copy audio data
    m_sfxIDs = source->m_sfxIDs;
    m_songIDs = source->m_songIDs;
    m_songSize = source->m_songSize;
}

GJGameLevel* GJGameLevel::create() {
    GJGameLevel* instance = new GJGameLevel();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

GJGameLevel* GJGameLevel::createWithCoder(DS_Dictionary* dict) {
    GJGameLevel* instance = GJGameLevel::create();
    if (instance) {
        instance->dataLoaded(dict);
    }
    return instance;
}

void GJGameLevel::dataLoaded(DS_Dictionary* dict) {
    m_levelNotDownloaded = dict->getBoolForKey("k35");
    
    int levelID = dict->getIntegerForKey("k1");
    m_levelID.value = levelID;
    m_levelID.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_levelID.random = levelID + m_levelID.seed;
    
    int attempts = dict->getIntegerForKey("k18");
    m_attempts.value = attempts;
    m_attempts.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_attempts.random = attempts + m_attempts.seed;
    
    int normalPercent = dict->getIntegerForKey("k19");
    setNormalPercent(normalPercent);
    
    int orbCompletion = dict->getIntegerForKey("k71");
    m_orbCompletion.value = orbCompletion;
    m_orbCompletion.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_orbCompletion.random = orbCompletion + m_orbCompletion.seed;
    
    int newNormalPercent2 = dict->getIntegerForKey("k90");
    m_newNormalPercent2.value = newNormalPercent2;
    m_newNormalPercent2.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_newNormalPercent2.random = newNormalPercent2 + m_newNormalPercent2.seed;
    
    m_practicePercent = dict->getIntegerForKey("k20");
    
    int jumps = dict->getIntegerForKey("k36");
    m_jumps.value = jumps;
    m_jumps.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_jumps.random = jumps + m_jumps.seed;
    
    int clicks = dict->getIntegerForKey("k85");
    m_clicks.value = clicks;
    m_clicks.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_clicks.random = clicks + m_clicks.seed;
    
    int attemptTime = dict->getIntegerForKey("k86");
    m_attemptTime.value = attemptTime;
    m_attemptTime.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_attemptTime.random = attemptTime + m_attemptTime.seed;
    
    m_chk = dict->getIntegerForKey("k87");
    m_personalBests = dict->getStringForKey("k88");
    m_isChkValid = dict->getBoolForKey("k89");
    m_levelLength = dict->getIntegerForKey("k23");
    
    bool demon = dict->getBoolForKey("k25");
    m_demon.value = demon;
    m_demon.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_demon.random = demon + m_demon.seed;
    
    m_demonDifficulty = dict->getIntegerForKey("k76");
    
    int stars = dict->getIntegerForKey("k26");
    m_stars.value = stars;
    m_stars.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_stars.random = stars + m_stars.seed;
    
    int dailyID = dict->getIntegerForKey("k74");
    m_dailyID.value = dailyID;
    m_dailyID.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_dailyID.random = dailyID + m_dailyID.seed;
    
    m_isCompletionLegitimate = dict->getBoolForKey("k68");
    m_gauntletLevel = dict->getBoolForKey("k77");
    m_gauntletLevel2 = dict->getBoolForKey("k78");
    m_levelFavorited = dict->getBoolForKey("k82");
    m_levelFolder = dict->getIntegerForKey("k84");
    m_bestTime = dict->getIntegerForKey("k107");
    m_ticksTime = dict->getIntegerForKey("k115");
    m_clicksTime = dict->getIntegerForKey("k117");
    m_coinsTime = dict->getIntegerForKey("k121");
    m_inputsTime = dict->getStringForKey("k119");
    m_bestPoints = dict->getIntegerForKey("k108");
    m_ticksPoints = dict->getIntegerForKey("k116");
    m_clicksPoints = dict->getIntegerForKey("k118");
    m_coinsPoints = dict->getIntegerForKey("k122");
    m_inputsPoints = dict->getStringForKey("k120");
    m_platformerSeed = dict->getIntegerForKey("k111");
    m_savedTime = dict->getBoolForKey("k123");
    m_savedPoints = dict->getBoolForKey("k124");
    m_localBestTimes = dict->getStringForKey("k109");
    m_localBestPoints = dict->getStringForKey("k110");
    m_cbsOverride = dict->getIntegerForKey("k125");
    
    if (m_levelNotDownloaded) {
        if (m_levelType == GJLevelType::Main) {
            m_isUnlocked = dict->getBoolForKey("k38");
            m_requiredCoins = dict->getIntegerForKey("k37");
        }
        return;
    }
    
    m_levelName = dict->getStringForKey("k2");
    m_levelDesc = dict->getStringForKey("k3");
    m_levelString = dict->getStringForKey("k4");
    m_creatorName = dict->getStringForKey("k5");
    m_timestamp = dict->getIntegerForKey("k95");
    m_lockedEditorLayers = dict->getStringForKey("k91");
    m_savedCameraPositions = dict->getStringForKey("k101");
    
    float x = dict->getFloatForKey("k102");
    float y = dict->getFloatForKey("k103");
    m_previewLock = cocos2d::CCPoint(x, y);
    
    m_coins = dict->getIntegerForKey("k64");
    
    bool coinsVerified = dict->getBoolForKey("k65");
    m_coinsVerified.value = coinsVerified;
    m_coinsVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_coinsVerified.random = coinsVerified + m_coinsVerified.seed;
    
    int userID = dict->getIntegerForKey("k6");
    m_userID.value = userID;
    m_userID.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_userID.random = userID + m_userID.seed;
    
    int accountID = dict->getIntegerForKey("k60");
    m_accountID.value = accountID;
    m_accountID.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_accountID.random = accountID + m_accountID.seed;
    
    m_difficulty = dict->getIntegerForKey("k7");
    m_audioTrack = dict->getIntegerForKey("k8");
    m_ratings = dict->getIntegerForKey("k9");
    m_ratingsSum = dict->getIntegerForKey("k10");
    m_downloads = dict->getIntegerForKey("k11");
    m_isEditable = dict->getBoolForKey("k13");
    
    bool verifiedRaw = dict->getBoolForKey("k14");
    m_isVerifiedRaw = verifiedRaw;
    m_isVerified.value = verifiedRaw;
    m_isVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_isVerified.random = verifiedRaw + m_isVerified.seed;
    
    m_isUploaded = dict->getBoolForKey("k15");
    m_likes = dict->getIntegerForKey("k22");
    m_dislikes = dict->getIntegerForKey("k24");
    m_levelType = static_cast<GJLevelType>(dict->getIntegerForKey("k21"));
    m_levelVersion = dict->getIntegerForKey("k16");
    m_gameVersion = dict->getIntegerForKey("k17");
    m_featured = dict->getIntegerForKey("k27");
    m_isEpic = dict->getIntegerForKey("k75");
    if (m_isEpic == 0) {
        m_isEpic = dict->getBoolForKey("k75") ? 1 : 0;
    }
    m_autoLevel = dict->getBoolForKey("k33");
    m_unlisted = dict->getBoolForKey("k79");
    m_friendsOnly = dict->getBoolForKey("k94");
    
    int password = dict->getIntegerForKey("k41");
    m_password.value = password;
    m_password.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_password.random = password + m_password.seed;
    
    int originalLevel = dict->getIntegerForKey("k42");
    m_originalLevel.value = originalLevel;
    m_originalLevel.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_originalLevel.random = originalLevel + m_originalLevel.seed;
    
    m_twoPlayerMode = dict->getBoolForKey("k43");
    m_songID = dict->getIntegerForKey("k45");
    m_levelRev = dict->getIntegerForKey("k46");
    m_hasBeenModified = dict->getBoolForKey("k47");
    
    int objectCount = dict->getIntegerForKey("k48");
    m_objectCount.value = objectCount;
    m_objectCount.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_objectCount.random = objectCount + m_objectCount.seed;
    
    m_capacityString = dict->getStringForKey("k67");
    m_starsRequested = dict->getIntegerForKey("k66");
    m_highObjectsEnabled = dict->getBoolForKey("k69");
    m_unlimitedObjectsEnabled = dict->getBoolForKey("k93");
    m_workingTime = dict->getIntegerForKey("k80");
    m_workingTime2 = dict->getIntegerForKey("k81");
    m_lowDetailMode = dict->getBoolForKey("k72");
    m_lowDetailModeToggled = dict->getBoolForKey("k73");
    m_disableShakeToggled = dict->getBoolForKey("k112");
    m_levelIndex = dict->getIntegerForKey("k83");
    m_songIDs = dict->getStringForKey("k104");
    m_sfxIDs = dict->getStringForKey("k105");
    m_songSize = dict->getIntegerForKey("k106");
    
    if (m_levelType == GJLevelType::Main) {
        m_isUnlocked = dict->getBoolForKey("k38");
        m_requiredCoins = dict->getIntegerForKey("k37");
    }
    
    if (m_levelType == GJLevelType::Editor) {
        m_lastCameraPos.x = dict->getFloatForKey("kI1");
        m_lastCameraPos.y = dict->getFloatForKey("kI2");
        m_lastEditorZoom = dict->getFloatForKey("kI3");
        m_lastBuildPage = dict->getIntegerForKey("kI4");
        m_lastBuildTab = dict->getIntegerForKey("kI5");
        m_lastBuildGroupID = dict->getIntegerForKey("kI7");
        
        cocos2d::CCDictionary* buildSave = dict->getDictForKey("kI6", nullptr);
        if (buildSave != m_lastBuildSave) {
            if (buildSave) {
                buildSave->retain();
            }
            if (m_lastBuildSave) {
                m_lastBuildSave->release();
            }
            m_lastBuildSave = buildSave;
        }
    }
    
    bool firstCoin = dict->getBoolForKey("k61");
    m_firstCoinVerified.value = firstCoin;
    m_firstCoinVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_firstCoinVerified.random = firstCoin + m_firstCoinVerified.seed;
    
    bool secondCoin = dict->getBoolForKey("k62");
    m_secondCoinVerified.value = secondCoin;
    m_secondCoinVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_secondCoinVerified.random = secondCoin + m_secondCoinVerified.seed;
    
    bool thirdCoin = dict->getBoolForKey("k63");
    m_thirdCoinVerified.value = thirdCoin;
    m_thirdCoinVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_thirdCoinVerified.random = thirdCoin + m_thirdCoinVerified.seed;
    
    m_recordString = dict->getStringForKey("k34");
}

int GJGameLevel::demonIconForDifficulty(DemonDifficultyType type) {
    int val = static_cast<int>(type) - 3;
    if (val >= 0 && val <= 3) {
        static int icons[] = {6, 7, 8, 9};
        return icons[val];
    }
    return 6;
}

void GJGameLevel::encodeWithCoder(DS_Dictionary* dict) {
    dict->setIntegerForKey("kCEK", 4);
    dict->setIntegerForKey("k1", m_levelID.random - m_levelID.seed);
    dict->setBoolForKey("k35", m_levelNotDownloaded);
    dict->setIntegerForKey("k18", m_attempts.random - m_attempts.seed);
    dict->setIntegerForKey("k36", m_jumps.random - m_jumps.seed);
    dict->setIntegerForKey("k85", m_clicks.random - m_clicks.seed);
    dict->setIntegerForKey("k86", m_attemptTime.random - m_attemptTime.seed);
    dict->setIntegerForKey("k87", m_chk);
    dict->setStringForKey("k88", m_personalBests);
    dict->setBoolForKey("k89", m_isChkValid);
    dict->setIntegerForKey("k23", m_levelLength);
    dict->setIntegerForKey("k19", getNormalPercent());
    dict->setIntegerForKey("k71", m_orbCompletion.random - m_orbCompletion.seed);
    dict->setIntegerForKey("k90", m_newNormalPercent2.random - m_newNormalPercent2.seed);
    dict->setIntegerForKey("k20", m_practicePercent);
    
    if (m_demon.random - m_demon.seed == 1) {
        dict->setBoolForKey("k25", true);
        dict->setIntegerForKey("k76", m_demonDifficulty);
    }
    
    int stars = m_stars.random - m_stars.seed;
    if (stars > 0) {
        dict->setIntegerForKey("k26", stars);
    }
    
    if (m_isCompletionLegitimate) {
        dict->setBoolForKey("k68", true);
    }
    
    dict->setIntegerForKey("k74", m_dailyID.random - m_dailyID.seed);
    dict->setBoolForKey("k77", m_gauntletLevel);
    dict->setBoolForKey("k78", m_gauntletLevel2);
    
    if (m_levelFavorited) {
        dict->setBoolForKey("k82", true);
    }
    
    if (m_levelFolder > 0) {
        dict->setIntegerForKey("k84", m_levelFolder);
    }
    
    if (m_bestTime > 0 || m_bestPoints > 0) {
        dict->setIntegerForKey("k107", m_bestTime);
        dict->setIntegerForKey("k115", m_ticksTime);
        dict->setIntegerForKey("k117", m_clicksTime);
        dict->setIntegerForKey("k121", m_coinsTime);
        if (m_savedTime) {
            dict->setStringForKey("k119", m_inputsTime);
        }
        dict->setIntegerForKey("k108", m_bestPoints);
        dict->setIntegerForKey("k116", m_ticksPoints);
        dict->setIntegerForKey("k118", m_clicksPoints);
        dict->setIntegerForKey("k122", m_coinsPoints);
        if (m_savedPoints) {
            dict->setStringForKey("k120", m_inputsPoints);
        }
        dict->setIntegerForKey("k111", m_platformerSeed);
        dict->setBoolForKey("k123", m_savedTime);
        dict->setBoolForKey("k124", m_savedPoints);
    }
    
    if (!m_localBestTimes.empty()) {
        dict->setStringForKey("k109", m_localBestTimes);
    }
    if (!m_localBestPoints.empty()) {
        dict->setStringForKey("k110", m_localBestPoints);
    }
    
    if (m_levelNotDownloaded) {
        return;
    }
    
    if (!m_levelName.empty()) {
        dict->setStringForKey("k2", m_levelName);
    } else if (!m_tempName.empty()) {
        m_levelName = m_tempName;
        dict->setStringForKey("k2", m_levelName);
    } else {
        m_levelName = "Unknown";
        dict->setStringForKey("k2", m_levelName);
    }
    
    if (!m_levelNotDownloaded && !m_dontSave && m_levelType != GJLevelType::Main && 
        (m_dailyID.random - m_dailyID.seed <= 0 || !GameManager::sharedState()->m_quickSave)) {
        // Save level data
        dict->setStringForKey("k4", m_levelString);
        dict->setStringForKey("k3", m_levelDesc);
    } else if (m_levelType == GJLevelType::Editor || (m_dailyID.random - m_dailyID.seed > 0 && !m_dontSave)) {
        dict->setStringForKey("k4", m_levelString);
        dict->setStringForKey("k3", m_levelDesc);
    } else {
        dict->setStringForKey("k4", "");
    }
    
    dict->setStringForKey("k5", m_creatorName);
    dict->setIntegerForKey("k95", m_timestamp);
    dict->setStringForKey("k91", m_lockedEditorLayers);
    dict->setStringForKey("k101", m_savedCameraPositions);
    dict->setIntegerForKey("k125", m_cbsOverride);
    
    if (m_previewLock.x != 0.0f || m_previewLock.y != 0.0f) {
        dict->setIntegerForKey("k102", static_cast<int>(m_previewLock.x));
        dict->setIntegerForKey("k103", static_cast<int>(m_previewLock.y));
    }
    
    dict->setIntegerForKey("k6", m_userID.random - m_userID.seed);
    dict->setIntegerForKey("k60", m_accountID.random - m_accountID.seed);
    dict->setIntegerForKey("k7", m_difficulty);
    dict->setIntegerForKey("k8", m_audioTrack);
    dict->setIntegerForKey("k9", m_ratings);
    dict->setIntegerForKey("k10", m_ratingsSum);
    dict->setIntegerForKey("k11", m_downloads);
    dict->setBoolForKey("k13", m_isEditable);
    dict->setBoolForKey("k14", (m_isVerified.random - m_isVerified.seed) == 1);
    dict->setBoolForKey("k15", m_isUploaded);
    dict->setIntegerForKey("k22", m_likes);
    dict->setIntegerForKey("k24", m_dislikes);
    dict->setIntegerForKey("k21", static_cast<int>(m_levelType));
    dict->setIntegerForKey("k16", m_levelVersion);
    dict->setIntegerForKey("k17", m_gameVersion);
    dict->setBoolForKey("k69", m_highObjectsEnabled);
    dict->setBoolForKey("k93", m_unlimitedObjectsEnabled);
    
    if (m_workingTime > 0) {
        dict->setIntegerForKey("k80", m_workingTime);
    }
    if (m_workingTime2 > 0) {
        dict->setIntegerForKey("k81", m_workingTime2);
    }
    
    dict->setBoolForKey("k72", m_lowDetailMode);
    dict->setBoolForKey("k73", m_lowDetailModeToggled);
    dict->setBoolForKey("k112", m_disableShakeToggled);
    dict->setIntegerForKey("k83", m_levelIndex);
    dict->setIntegerForKey("k64", m_coins);
    dict->setBoolForKey("k65", (m_coinsVerified.random - m_coinsVerified.seed) == 1);
    dict->setIntegerForKey("k27", m_featured);
    dict->setIntegerForKey("k75", m_isEpic);
    dict->setBoolForKey("k33", m_autoLevel);
    dict->setBoolForKey("k79", m_unlisted);
    dict->setBoolForKey("k94", m_friendsOnly);
    
    dict->setIntegerForKey("k41", m_password.random - m_password.seed);
    dict->setIntegerForKey("k42", m_originalLevel.random - m_originalLevel.seed);
    dict->setBoolForKey("k43", m_twoPlayerMode);
    dict->setIntegerForKey("k45", m_songID);
    dict->setIntegerForKey("k50", 48);
    dict->setIntegerForKey("k46", m_levelRev);
    dict->setBoolForKey("k47", m_hasBeenModified);
    dict->setIntegerForKey("k48", m_objectCount.random - m_objectCount.seed);
    dict->setStringForKey("k67", m_capacityString);
    
    dict->setStringForKey("k104", m_songIDs);
    dict->setStringForKey("k105", m_sfxIDs);
    dict->setIntegerForKey("k106", m_songSize);
    dict->setIntegerForKey("k66", m_starsRequested);
    
    if (m_levelType == GJLevelType::Main) {
        dict->setBoolForKey("k38", m_isUnlocked);
        dict->setIntegerForKey("k37", m_requiredCoins);
    }
    
    if (m_levelType == GJLevelType::Editor) {
        dict->setFloatForKey("kI1", m_lastCameraPos.x);
        dict->setFloatForKey("kI2", m_lastCameraPos.y);
        dict->setFloatForKey("kI3", m_lastEditorZoom);
        dict->setIntegerForKey("kI4", m_lastBuildPage);
        dict->setIntegerForKey("kI5", m_lastBuildTab);
        dict->setIntegerForKey("kI7", m_lastBuildGroupID);
        dict->setDictForKey("kI6", m_lastBuildSave);
        dict->setBoolForKey("k61", (m_firstCoinVerified.random - m_firstCoinVerified.seed) == 1);
        dict->setBoolForKey("k62", (m_secondCoinVerified.random - m_secondCoinVerified.seed) == 1);
        dict->setBoolForKey("k63", (m_thirdCoinVerified.random - m_thirdCoinVerified.seed) == 1);
        dict->setStringForKey("k34", m_recordString);
    }
}

std::string GJGameLevel::generateSettingsString() {
    return "";
}

std::string GJGameLevel::getAudioFileName() {
    if (m_songID > 0) {
        MusicDownloadManager* manager = MusicDownloadManager::sharedState();
        return manager->pathForSong(m_songID);
    }
    return LevelTools::getAudioFileName(m_audioTrack);
}

int GJGameLevel::getAverageDifficulty() {
    if (m_ratings > 4) {
        return static_cast<int>(std::round(static_cast<float>(m_ratingsSum) / m_ratings));
    }
    return 0;
}

const char* GJGameLevel::getCoinKey(int coinNumber) {
    int levelID = m_levelID.random - m_levelID.seed;
    int dailyID = m_dailyID.random - m_dailyID.seed;
    
    if (dailyID > 0) {
        return cocos2d::CCString::createWithFormat("%i_%i_%i", levelID, coinNumber, dailyID)->getCString();
    }
    if (m_gauntletLevel) {
        return cocos2d::CCString::createWithFormat("%i_%i_g", levelID, coinNumber)->getCString();
    }
    return cocos2d::CCString::createWithFormat("%i_%i", levelID, coinNumber)->getCString();
}

int GJGameLevel::getLastBuildPageForTab(int tab) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tab);
    cocos2d::CCString* value = static_cast<cocos2d::CCString*>(m_lastBuildSave->objectForKey(key->getCString()));
    if (value) {
        return value->intValue();
    }
    return 0;
}

int GJGameLevel::getLengthKey(int length, bool platformer) {
    if (platformer) {
        return 5;
    }
    if (length >= 120) {
        return 4;
    }
    if (length >= 60) {
        return 3;
    }
    if (length >= 30) {
        return 2;
    }
    if (length >= 10) {
        return 1;
    }
    return 0;
}

GJGameLevel* GJGameLevel::getListSnapshot() {
    GJGameLevel* snapshot = GJGameLevel::create();
    
    snapshot->m_levelID = m_levelID;
    snapshot->m_levelName = m_levelName;
    snapshot->m_levelLength = m_levelLength;
    snapshot->m_timestamp = m_timestamp;
    snapshot->m_accountID = m_accountID;
    snapshot->m_creatorName = m_creatorName;
    snapshot->m_likes = m_likes;
    snapshot->m_downloads = m_downloads;
    snapshot->m_audioTrack = m_audioTrack;
    snapshot->m_songID = m_songID;
    snapshot->m_levelType = m_levelType;
    snapshot->m_ratings = m_ratings;
    snapshot->m_ratingsSum = m_ratingsSum;
    snapshot->m_demon = m_demon;
    snapshot->m_autoLevel = m_autoLevel;
    snapshot->m_demonDifficulty = m_demonDifficulty;
    snapshot->m_featured = m_featured;
    snapshot->m_isEpic = m_isEpic;
    snapshot->m_stars = m_stars;
    snapshot->m_coins = m_coins;
    snapshot->m_coinsVerified = m_coinsVerified;
    snapshot->m_originalLevel = m_originalLevel;
    snapshot->m_objectCount = m_objectCount;
    
    return snapshot;
}

int GJGameLevel::getNormalPercent() {
    return m_normalPercent.random - m_normalPercent.seed;
}

std::string GJGameLevel::getSongName() {
    std::string result;
    if (m_songID > 0) {
        SongInfoObject* info = SongInfoObject::create(m_songID);
        if (info && !info->m_isUnknown) {
            result = info->m_songName;
        } else {
            result = "Unknown";
        }
    } else {
        result = LevelTools::getAudioTitle(m_audioTrack);
    }
    return result;
}

std::string GJGameLevel::getUnpackedLevelDescription() {
    std::string result;
    if (!m_levelDesc.empty()) {
        std::string decoded = LevelTools::base64DecodeString(m_levelDesc);
        result = decoded;
    } else {
        result = "";
    }
    return result;
}

void GJGameLevel::handleStatsConflict(GJGameLevel* other) {
    // Merge attempts
    int attempts = other->m_attempts.random - other->m_attempts.seed;
    int myAttempts = m_attempts.random - m_attempts.seed;
    if (myAttempts >= attempts) {
        attempts = myAttempts;
    }
    m_attempts.value = attempts;
    m_attempts.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_attempts.random = m_attempts.seed + attempts;
    
    // Merge jumps
    int jumps = other->m_jumps.random - other->m_jumps.seed;
    int myJumps = m_jumps.random - m_jumps.seed;
    if (myJumps >= jumps) {
        jumps = myJumps;
    }
    m_jumps.value = jumps;
    m_jumps.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_jumps.random = m_jumps.seed + jumps;
    
    // Merge clicks
    int clicks = other->m_clicks.random - other->m_clicks.seed;
    int myClicks = m_clicks.random - m_clicks.seed;
    if (myClicks >= clicks) {
        clicks = myClicks;
    }
    m_clicks.value = clicks;
    m_clicks.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_clicks.random = m_clicks.seed + clicks;
    
    // Merge normal percent
    int myNormalPercent = getNormalPercent();
    int otherNormalPercent = other->getNormalPercent();
    if (otherNormalPercent > myNormalPercent) {
        setNormalPercent(otherNormalPercent);
    } else {
        setNormalPercent(myNormalPercent);
    }
    
    // Merge orb completion
    int orbCompletion = m_orbCompletion.random - m_orbCompletion.seed;
    int otherOrbCompletion = other->m_orbCompletion.random - other->m_orbCompletion.seed;
    if (otherOrbCompletion > orbCompletion) {
        orbCompletion = otherOrbCompletion;
    }
    m_orbCompletion.value = orbCompletion;
    m_orbCompletion.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_orbCompletion.random = m_orbCompletion.seed + orbCompletion;
    
    // Merge new normal percent 2
    int newNormalPercent2 = m_newNormalPercent2.random - m_newNormalPercent2.seed;
    int otherNewNormalPercent2 = other->m_newNormalPercent2.random - other->m_newNormalPercent2.seed;
    
    // Save time score from other
    bool savedTime = saveNewScore(
        other->m_bestTime,
        0,
        other->m_ticksTime,
        other->m_clicksTime,
        other->m_coinsTime,
        other->m_inputsTime,
        false
    );
    if (savedTime) {
        m_savedTime = other->m_savedTime;
    }
    
    // Save points score from other
    bool savedPoints = saveNewScore(
        other->m_bestPoints,
        1,
        other->m_ticksPoints,
        other->m_clicksPoints,
        other->m_coinsPoints,
        other->m_inputsPoints,
        false
    );
    if (savedPoints) {
        m_savedPoints = other->m_savedPoints;
    }
    
    if (newNormalPercent2 < otherNewNormalPercent2) {
        m_newNormalPercent2.value = otherNewNormalPercent2;
        m_newNormalPercent2.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
        m_newNormalPercent2.random = otherNewNormalPercent2 + m_newNormalPercent2.seed;
        
        // Merge clicks
        int otherClicks = other->m_clicks.random - other->m_clicks.seed;
        m_clicks.value = otherClicks;
        m_clicks.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
        m_clicks.random = otherClicks + m_clicks.seed;
        
        // Merge attempt time
        int otherAttemptTime = other->m_attemptTime.random - other->m_attemptTime.seed;
        m_attemptTime.value = otherAttemptTime;
        m_attemptTime.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
        m_attemptTime.random = otherAttemptTime + m_attemptTime.seed;
        
        m_chk = other->m_chk;
        m_personalBests = other->m_personalBests;
        m_isChkValid = other->m_isChkValid;
    }
    
    // Merge practice percent
    int practicePercent = m_practicePercent;
    if (other->m_practicePercent > practicePercent) {
        practicePercent = other->m_practicePercent;
    }
    m_practicePercent = practicePercent;
    
    // Merge level index
    int levelIndex = m_levelIndex;
    if (other->m_levelIndex > levelIndex) {
        levelIndex = other->m_levelIndex;
    }
    m_levelIndex = levelIndex;
}


bool GJGameLevel::init() {
    m_levelName = "";
    m_levelDesc = "";
    m_levelString = "";
    m_personalBests = "";
    m_levelType = GJLevelType::Editor;
    m_levelVersion = 1;
    
    m_attempts.value = 0;
    m_M_ID = 0;
    m_attempts.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_attempts.random = m_attempts.seed;
    
    m_jumps.value = 0;
    m_jumps.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_jumps.random = m_jumps.seed;
    
    m_clicks.value = 0;
    m_clicks.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_clicks.random = m_clicks.seed;
    
    m_attemptTime.value = 0;
    m_attemptTime.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_attemptTime.random = m_attemptTime.seed;
    
    m_chk = 0;
    m_coins = 0;
    m_stars.value = 0;
    m_stars.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_stars.random = m_stars.seed;
    
    m_demon.value = 0;
    m_demon.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_demon.random = m_demon.seed;
    
    m_dontSave = false;
    m_levelNotDownloaded = false;
    
    cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
    if (dict != m_lastBuildSave) {
        if (dict) {
            dict->retain();
        }
        if (m_lastBuildSave) {
            m_lastBuildSave->release();
        }
        m_lastBuildSave = dict;
    }
    
    return true;
}

bool GJGameLevel::isPlatformer() {
    return m_levelLength == 5;
}

std::string GJGameLevel::lengthKeyToString(int key) {
    switch (key) {
        case 0: return "Tiny";
        case 1: return "Short";
        case 2: return "Medium";
        case 3: return "Long";
        case 5: return "Plat.";
        default: return "XL";
    }
}

void GJGameLevel::levelWasAltered() {
    m_isVerifiedRaw = false;
    m_hasBeenModified = true;
    m_isVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_isVerified.random = m_isVerified.seed;
    unverifyCoins();
    m_capacityString = "";
    m_bestTime = 0;
    m_bestPoints = 0;
    m_savedTime = false;
    m_savedPoints = false;
    if (m_isUploaded) {
        m_isUploaded = false;
        m_levelVersion++;
    }
}

void GJGameLevel::levelWasSubmitted() {
    m_isUploaded = true;
}

void GJGameLevel::parseSettingsString(std::string str) {
    // Parse settings string from server
}

void GJGameLevel::scoreStringToVector(std::string* str, std::vector<int>* vec) {
    if (str->empty()) {
        return;
    }
    
    // Parse comma-separated values from string
    size_t pos = 0;
    size_t start = 0;
    while ((pos = str->find(',', start)) != std::string::npos) {
        int value = std::stoi(str->substr(start, pos - start));
        vec->push_back(value);
        start = pos + 1;
    }
    if (start < str->length()) {
        int value = std::stoi(str->substr(start));
        vec->push_back(value);
    }
}

std::string GJGameLevel::scoreVectorToString(std::vector<int>* vec, int type) {
    std::string result;
    
    if (type == 1) {
        // Sort descending for points
        std::sort(vec->begin(), vec->end(), std::greater<int>());
    } else {
        // Sort ascending for times
        std::sort(vec->begin(), vec->end());
    }
    
    // Limit to 10 entries
    if (vec->size() > 10) {
        vec->resize(10);
    }
    
    for (size_t i = 0; i < vec->size(); ++i) {
        if (i > 0) {
            result += ",";
        }
        result += std::to_string((*vec)[i]);
    }
    
    return result;
}

void GJGameLevel::setLastBuildPageForTab(int tab, int page) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tab);
    cocos2d::CCString* value = cocos2d::CCString::createWithFormat("%i", page);
    m_lastBuildSave->setObject(value, key->getCString());
}

void GJGameLevel::setNormalPercent(int percent) {
    m_normalPercent.value = percent;
    m_normalPercent.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_normalPercent.random = percent + m_normalPercent.seed;
}

bool GJGameLevel::shouldCheatReset() {
    if (m_stars.random - m_stars.seed > 7 && m_jumps.random - m_jumps.seed <= 49) {
        return (m_clicks.random - m_clicks.seed) <= 49;
    }
    return false;
}

void GJGameLevel::storeNewLocalScore(int value, int type) {
    std::string* target = (type == 1) ? &m_localBestPoints : &m_localBestTimes;
    std::vector<int> scores;
    
    scoreStringToVector(target, &scores);
    
    // Check if value already exists
    auto it = std::find(scores.begin(), scores.end(), value);
    if (it == scores.end()) {
        scores.push_back(value);
        *target = scoreVectorToString(&scores, type);
    }
}

void GJGameLevel::unverifyCoins() {
    m_firstCoinVerified.value = false;
    m_firstCoinVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_firstCoinVerified.random = m_firstCoinVerified.seed;
    
    m_secondCoinVerified.value = false;
    m_secondCoinVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_secondCoinVerified.random = m_secondCoinVerified.seed;
    
    m_thirdCoinVerified.value = false;
    m_thirdCoinVerified.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
    m_thirdCoinVerified.random = m_thirdCoinVerified.seed;
}

GJGameLevel::~GJGameLevel() {
    if (m_lastBuildSave) {
        m_lastBuildSave->release();
    }
}