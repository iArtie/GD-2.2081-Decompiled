#include "Managers/GameManager.h"
#include "Utils/DS_Dictionary.h"
#include "Managers/GJAccountManager.h"
#include "Managers/GameStatsManager.h"
#include "GameLevelManager.h"
#include "Managers/LocalLevelManager.h"
#include "Managers/AchievementManager.h"
#include "Managers/MusicDownloadManager.h"
#include "Managers/KeybindingsManager.h"
#include "Managers/GJMultiplayerManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Utils/PlatformToolbox.h"
#include "Delegates/AppDelegate.h"
#include "Utils/AdToolbox.h"
#include "Utils/GameToolbox.h"
#include "Editor/ObjectDecoder.h"
#include "Editor/GJObjectDecoder.h"
#include "Layers/MenuLayer.h"
#include "Layers/PlayLayer.h"
#include "Layers/LevelEditorLayer.h"
#include "Layers/ShaderLayer.h"
#include "Sprites/SimplePlayer.h"
#include "Layers/FLAlertLayer.h"
#include "Models/GJGameLevel.h"
#include "Utils/LevelTools.h"
#include "CCObject.h"

GameManager* GameManager::sharedState()
{
    static GameManager* instance = nullptr;
    if (!instance)
    {
        instance = new GameManager();
        instance->init();
    }
    return instance;
}

bool GameManager::init()
{
    m_fileName = "CCGameManager.dat";
    m_unkBool2 = false;
    m_googlePlaySignedIn = false;
    m_vsyncEnabled = false;
    m_adTimer = 0.0f;
    m_unkDouble2 = 0.0f;
    
    m_gjLog = cocos2d::CCArray::create();
    m_gjLog->retain();
    
    setup();
    calculateBaseKeyForIcons();
    setupGameAnimations();
    
    int randVal = rand();
    float value = floorf((float)(m_playerUserID.random - m_playerUserID.seed) / 10000.0f);
    float value2 = (float)rand() * 4.6566e-10f * 1000.0f;
    m_chk.seed = (int)value2;
    m_chk.random = (int)(value + (float)((float)randVal * 4.6566e-10f * 1000000.0f)) + (int)value2;
    
    return true;
}

void GameManager::setup()
{
}

void GameManager::firstLoad()
{
    m_clickedYouTube = false;
    m_clickedFacebook = false;
    m_clickedTwitter = false;
    m_clickedTwitch = false;
    m_clickedDiscord = false;
    m_clickedReddit = false;
    m_socialsDuration = 0.0;
    
    resetDPadSettings(false);
    resetDPadSettings(true);
    
    if (!m_shouldLoadUnlockValueKeeper)
    {
        if (!m_valueKeeper)
        {
            m_valueKeeper = cocos2d::CCDictionary::create();
            m_valueKeeper->retain();
        }
        if (!m_unlockValueKeeper)
        {
            m_unlockValueKeeper = cocos2d::CCDictionary::create();
            m_unlockValueKeeper->retain();
        }
        if (!m_customObjectDict)
        {
            m_customObjectDict = cocos2d::CCDictionary::create();
            m_customObjectDict->retain();
        }
        
        PlatformToolbox::getUniqueUserID();
        m_playerUDID = PlatformToolbox::getUniqueUserID();
    }
    else
    {

        cocos2d::CCDictionary* valueDict = cocos2d::CCDictionary::create();
        if (valueDict != m_valueKeeper)
        {
            if (valueDict) valueDict->retain();
            if (m_valueKeeper) m_valueKeeper->release();
            m_valueKeeper = valueDict;
        }
        
        cocos2d::CCDictionary* unlockDict = cocos2d::CCDictionary::create();
        if (unlockDict != m_unlockValueKeeper)
        {
            if (unlockDict) unlockDict->retain();
            if (m_unlockValueKeeper) m_unlockValueKeeper->release();
            m_unlockValueKeeper = unlockDict;
        }
        
        if (!m_customObjectDict)
        {
            m_customObjectDict = cocos2d::CCDictionary::create();
            m_customObjectDict->retain();
        }
    }
    
    m_playerName = "Player";
    m_playerUserID.value = 0;
    m_playerUserID.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerUserID.random = m_playerUserID.seed;
    
    m_hasRP.value = 0;
    m_hasRP.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_hasRP.random = m_hasRP.seed;
    
    m_hasDRP = false;
    
    if (!m_shouldLoadUnlockValueKeeper)
    {
        m_firstSetup = true;
        m_showedMenu = true;
        m_unknownBool4 = true;
    }
    
    resetAllIcons();
    
    if (!m_shouldLoadUnlockValueKeeper)
    {
        FMODAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0f);
        FMODAudioEngine::sharedEngine()->setEffectsVolume(1.0f);
    }
    
    AchievementManager::sharedState()->firstSetup();
    GameLevelManager::sharedState()->firstSetup();
    GameStatsManager::sharedState()->firstSetup();
    MusicDownloadManager::sharedState()->firstSetup();
    GJAccountManager::sharedState()->firstSetup();
    GJMultiplayerManager::sharedState()->firstSetup();
    KeybindingsManager::sharedState()->firstSetup();
    
    bool shouldLoad = m_shouldLoadUnlockValueKeeper;
    
    if (m_shouldLoadUnlockValueKeeper)
    {
        if (m_valueKeeper)
        {
            cocos2d::CCArray* keys = m_valueKeeper->allKeys();
            for (unsigned int i = 0; i < keys->count(); i++)
            {
                cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
                cocos2d::CCString* value = (cocos2d::CCString*)m_valueKeeper->valueForKey(key->getCString());
                if (value)
                {
                    setIntGameVariable(key->getCString(), value->intValue());
                }
            }
            shouldLoad = m_shouldLoadUnlockValueKeeper;
        }
    }
    else
    {
        m_resolution = -1;
        m_showSongMarkers = true;
        m_showBPMMarkers = false;
        m_showProgressBar = false;
        m_performanceMode = false;
        m_vsyncEnabled = true;
        m_showedRateDiffDialog = false;
        m_showedRateStarDialog = false;
        m_showedLowDetailDialog = false;
        m_recordGameplay = false;
        
        setGameVariable("0002", true);
        setGameVariable("0001", true);
        setGameVariable("0026", true);
        setGameVariable("0027", true);
        setGameVariable("0023", true);
        setGameVariable("0038", true);
        setGameVariable("0043", true);
        setGameVariable("0044", true);
        setIntGameVariable("0050", 2);
        setIntGameVariable("0049", 6);
        setGameVariable("0046", true);
        setGameVariable("0036", true);
        setGameVariable("0118", true);
        setGameVariable("0117", true);
        setGameVariable("0158", true);
        setGameVariable("0121", true);
        setGameVariable("0037", true);
        shouldLoad = m_shouldLoadUnlockValueKeeper;
    }
    
    if (shouldLoad && getUGV("30"))
    {
        setUGV("30", true);
    }
    
    m_loaded = true;
}

void GameManager::dataLoaded(DS_Dictionary* dict)
{
    ObjectDecoder* decoder = ObjectDecoder::sharedDecoder();
    GJObjectDecoder* gjDecoder = GJObjectDecoder::sharedDecoder();
    if (gjDecoder)
    {
        decoder->m_delegate = &gjDecoder->ObjectDecoderDelegate;
    }
    else
    {
        decoder->m_delegate = nullptr;
    }
    
    if (!m_valueKeeper)
    {
        cocos2d::CCDictionary* valueDict = dict->getDictForKey("valueKeeper", false);
        if (valueDict != m_valueKeeper)
        {
            if (valueDict) valueDict->retain();
            if (m_valueKeeper) m_valueKeeper->release();
            m_valueKeeper = valueDict;
        }
    }
    
    cocos2d::CCDictionary* unlockDict = dict->getDictForKey("unlockValueKeeper", false);
    if (m_unlockValueKeeper)
    {
        GameToolbox::mergeDictsSaveLargestInt(m_unlockValueKeeper, unlockDict);
    }
    else
    {
        if (unlockDict)
        {
            unlockDict->retain();
            m_unlockValueKeeper = unlockDict;
        }
    }
    
    if (!m_customObjectDict)
    {
        cocos2d::CCDictionary* customDict = dict->getDictForKey("customObjectDict", false);
        if (customDict != m_customObjectDict)
        {
            if (customDict) customDict->retain();
            if (m_customObjectDict) m_customObjectDict->release();
            m_customObjectDict = customDict;
        }
    }
    
    m_playerUDID = dict->getStringForKey("playerUDID");
    m_playerName = dict->getStringForKey("playerName");
    
    int userID = dict->getIntegerForKey("playerUserID");
    m_playerUserID.value = userID;
    m_playerUserID.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerUserID.random = userID + m_playerUserID.seed;
    
    int frame = dict->getIntegerForKey("playerFrame");
    m_playerFrame.value = frame;
    m_playerFrame.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerFrame.random = frame + m_playerFrame.seed;
    
    int color = dict->getIntegerForKey("playerColor");
    m_playerColor.value = color;
    m_playerColor.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerColor.random = color + m_playerColor.seed;
    
    int color2 = dict->getIntegerForKey("playerColor2");
    m_playerColor2.value = color2;
    m_playerColor2.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerColor2.random = color2 + m_playerColor2.seed;
    
    int glowColor = dict->getIntegerForKey("playerColor3");
    m_playerGlowColor.value = glowColor;
    m_playerGlowColor.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerGlowColor.random = glowColor + m_playerGlowColor.seed;
    
    int ship = dict->getIntegerForKey("playerShip");
    if (ship > 1) m_playerShip.value = ship;
    else m_playerShip.value = 1;
    m_playerShip.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerShip.random = m_playerShip.value + m_playerShip.seed;
    
    int ball = dict->getIntegerForKey("playerBall");
    if (ball > 1) m_playerBall.value = ball;
    else m_playerBall.value = 1;
    m_playerBall.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerBall.random = m_playerBall.value + m_playerBall.seed;
    
    int bird = dict->getIntegerForKey("playerBird");
    if (bird > 1) m_playerBird.value = bird;
    else m_playerBird.value = 1;
    m_playerBird.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerBird.random = m_playerBird.value + m_playerBird.seed;
    
    int dart = dict->getIntegerForKey("playerDart");
    if (dart > 1) m_playerDart.value = dart;
    else m_playerDart.value = 1;
    m_playerDart.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerDart.random = m_playerDart.value + m_playerDart.seed;
    
    int robot = dict->getIntegerForKey("playerRobot");
    if (robot > 1) m_playerRobot.value = robot;
    else m_playerRobot.value = 1;
    m_playerRobot.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerRobot.random = m_playerRobot.value + m_playerRobot.seed;
    
    int spider = dict->getIntegerForKey("playerSpider");
    if (spider > 1) m_playerSpider.value = spider;
    else m_playerSpider.value = 1;
    m_playerSpider.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerSpider.random = m_playerSpider.value + m_playerSpider.seed;
    
    int jetpack = dict->getIntegerForKey("playerJetpack");
    if (jetpack > 1) m_playerJetpack.value = jetpack;
    else m_playerJetpack.value = 1;
    m_playerJetpack.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerJetpack.random = m_playerJetpack.value + m_playerJetpack.seed;
    
    int swing = dict->getIntegerForKey("playerSwing");
    if (swing > 1) m_playerSwing.value = swing;
    else m_playerSwing.value = 1;
    m_playerSwing.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerSwing.random = m_playerSwing.value + m_playerSwing.seed;
    
    int streak = dict->getIntegerForKey("playerStreak");
    if (streak > 1) m_playerStreak.value = streak;
    else m_playerStreak.value = 1;
    m_playerStreak.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerStreak.random = m_playerStreak.value + m_playerStreak.seed;
    
    int shipFire = dict->getIntegerForKey("playerShipStreak");
    if (shipFire > 1) m_playerShipFire.value = shipFire;
    else m_playerShipFire.value = 1;
    m_playerShipFire.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerShipFire.random = m_playerShipFire.value + m_playerShipFire.seed;
    
    int deathEffect = dict->getIntegerForKey("playerDeathEffect");
    if (deathEffect > 1) m_playerDeathEffect.value = deathEffect;
    else m_playerDeathEffect.value = 1;
    m_playerDeathEffect.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerDeathEffect.random = m_playerDeathEffect.value + m_playerDeathEffect.seed;
    
    m_playerGlow = dict->getBoolForKey("playerGlow");
    m_playerIconType = dict->getIntegerForKey("playerIconType");
    
    int secretNum = dict->getIntegerForKey("secretNumber");
    m_secretNumber.seed = secretNum;
    m_secretNumber.random = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_secretNumber.random += secretNum;
    
    m_disableThumbstick = getGameVariable("0028");
    
    AchievementManager::sharedState()->dataLoaded(dict);
    GameStatsManager::sharedState()->dataLoaded(dict);
    GameLevelManager::sharedState()->dataLoaded(dict);
    MusicDownloadManager::sharedState()->dataLoaded(dict);
    KeybindingsManager::sharedState()->dataLoaded(dict);
    
    int binaryVersion = dict->getIntegerForKey("binaryVersion");
    
    AchievementManager::sharedState()->m_dontNotify = true;
    GameStatsManager::sharedState()->recountSpecialStats();
    
    if (!m_loaded)
    {
        GameStatsManager::sharedState()->tryFixPathBug();
        GameStatsManager::sharedState()->verifyPathAchievements();
    }
    
    if (!GameStatsManager::sharedState()->isItemUnlocked(GJItem, 16))
    {
        m_customMenuSongID = 0;
        m_customPracticeSongID = 0;
    }
    
    m_showSongMarkers = dict->getBoolForKey("showSongMarkers");
    m_showBPMMarkers = dict->getBoolForKey("showBPMMarkers");
    m_showProgressBar = dict->getBoolForKey("showProgressBar");
    m_clickedEditor = dict->getBoolForKey("clickedEditor");
    m_clickedGarage = dict->getBoolForKey("clickedGarage");
    m_clickedName = dict->getBoolForKey("clickedName");
    m_clickedPractice = dict->getBoolForKey("clickedPractice");
    m_showedEditorGuide = dict->getBoolForKey("showedEditorGuide");
    m_showedRateDiffDialog = dict->getBoolForKey("showedRateDiffDialog");
    m_showedRateStarDialog = dict->getBoolForKey("showedRateStarDialog");
    m_showedLowDetailDialog = dict->getBoolForKey("showedLowDetailDialog");
    m_hasRatedGame = dict->getBoolForKey("hasRatedGame");
    
    if (!m_loaded)
    {
        m_performanceMode = dict->getBoolForKey("performanceMode");
        m_recordGameplay = dict->getBoolForKey("recordGameplay");
        m_bootups = dict->getIntegerForKey("bootups") + 1;
        m_bgVolume = dict->getFloatForKey("bgVolume");
        m_sfxVolume = dict->getFloatForKey("sfxVolume");
        m_timeOffset = (float)dict->getIntegerForKey("timeOffset");
        m_resolution = dict->getIntegerForKey("resolution");
        m_texQuality = dict->getIntegerForKey("texQuality");
        m_customFPSTarget = dict->getFloatForKey("customFPSTarget");
        m_customMenuSongID = dict->getIntegerForKey("customMenuSongID");
        m_customPracticeSongID = dict->getIntegerForKey("customPracticeSongID");
        
        m_dpadLayout1 = dict->getStringForKey("dpadLayout01");
        m_dpadLayout2 = dict->getStringForKey("dpadLayout02");
        m_dpadLayout3 = dict->getStringForKey("dpadLayout03");
        m_dpadLayoutDual1 = dict->getStringForKey("dpadLayoutDual01");
        m_dpadLayoutDual2 = dict->getStringForKey("dpadLayoutDual02");
        m_dpadLayoutDual3 = dict->getStringForKey("dpadLayoutDual03");
        
        m_practiceOpacity = dict->getFloatForKey("practiceOpacity");
        float practiceX = dict->getFloatForKey("practicePosX");
        float practiceY = dict->getFloatForKey("practicePosY");
        m_practicePos = cocos2d::CCPoint(practiceX, practiceY);
        
        GJAccountManager::sharedState()->dataLoaded(dict);
        GJMultiplayerManager::sharedState()->dataLoaded(dict);
    }
    
    if (binaryVersion <= 39)
    {
        m_timeOffset = 0.0f;
    }
    
    prepareDPadSettings();
    
    if (GJAccountManager::sharedState()->m_accountID > 0)
    {
        GameLevelManager::sharedState()->m_leaderboardType = Friends;
        m_playerName = GJAccountManager::sharedState()->m_username;
    }
    
    int hasRP = dict->getIntegerForKey("hasRP");
    m_hasRP.value = hasRP;
    m_hasRP.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_hasRP.random = hasRP + m_hasRP.seed;
    
    m_hasDRP = dict->getBoolForKey("hasDRP");
    
    if (!m_loaded)
    {
        if (binaryVersion > 20)
        {
            verifyAchievementUnlocks();
        }
        else
        {
            verifyCoinUnlocks();
        }
        
        if (binaryVersion <= 3)
        {
            verifyStarUnlocks();
        }
        
        if (binaryVersion != 11)
        {
            if (binaryVersion > 47)
            {
                // Most recent version
            }
        }
        else
        {
            setGameVariable("0002", true);
            setGameVariable("0001", true);
        }
        
  
        GameStatsManager::sharedState()->incrementStat("3", 0);
        GameStatsManager::sharedState()->incrementStat("6", 0);
        GameStatsManager::sharedState()->incrementStat("5", 0);
        GameStatsManager::sharedState()->incrementStat("8", 0);
        GameStatsManager::sharedState()->incrementStat("15", 0);
        GameStatsManager::sharedState()->incrementStat("40", 0);
        GameStatsManager::sharedState()->incrementStat("42", 0);
        GameStatsManager::sharedState()->incrementStat("8", 0);
        GameStatsManager::sharedState()->incrementStat("1", 0);
        GameStatsManager::sharedState()->incrementStat("7", 0);
        GameStatsManager::sharedState()->incrementStat("12", 0);
        GameStatsManager::sharedState()->incrementStat("13", 0);
        GameStatsManager::sharedState()->incrementStat("41", 0);
        
        if (binaryVersion <= 22)
        {
            m_bgVolume = 1.0f;
            m_sfxVolume = 1.0f;
            setGameVariable("0026", true);
            setGameVariable("0027", true);
        }
        
        if (binaryVersion <= 24)
        {
            setGameVariable("0034", dict->getBoolForKey("gameCenterEnabled"));
            setGameVariable("0038", true);
            setGameVariable("0043", true);
            setGameVariable("0046", true);
            setGameVariable("0044", true);
            setGameVariable("0036", true);
            
            if (m_playerStreak.random == 2)
            {
                m_playerGlow = true;
            }
            
            GJGameLevel* level14 = GameLevelManager::sharedState()->getMainLevel(14, false);
            GameStatsManager::sharedState()->checkCoinAchievement(level14);
            
            GJGameLevel* level18 = GameLevelManager::sharedState()->getMainLevel(18, false);
            GameStatsManager::sharedState()->checkCoinAchievement(level18);
            
            GJGameLevel* level20 = GameLevelManager::sharedState()->getMainLevel(20, false);
            GameStatsManager::sharedState()->checkCoinAchievement(level20);
        }
        
        if (binaryVersion <= 28)
        {
            if (!m_loaded)
            {
                GameStatsManager::sharedState()->verifyUserCoins();
            }
        }
        
        if (binaryVersion <= 37)
        {
            setGameVariable("0012", false);
            setGameVariable("0016", false);
            setUGV("28", false);
            setUGV("29", false);
            setGameVariable("0118", true);
            setGameVariable("0117", true);
            setGameVariable("0158", true);
            setGameVariable("0121", true);
            setGameVariable("0037", true);
        }
        
        if (binaryVersion <= 40)
        {
            if (!m_loaded)
            {
                m_practiceOpacity = 1.0f;
            }
        }
        
        if (binaryVersion <= 44)
        {
            recountUserStats("");
        }
    }
    
    checkSteamAchievementUnlock();
    AchievementManager::sharedState()->m_dontNotify = false;
    
    if (getIntGameVariable("0049") <= 5)
    {
        setIntGameVariable("0049", 6);
        if (getIntGameVariable("0050") > 1)
        {
            // aLready setting
        }
        else
        {
            setIntGameVariable("0050", 2);
        }
    }
    else if (getIntGameVariable("0050") <= 1)
    {
        setIntGameVariable("0050", 2);
    }
    
    m_loaded = true;
}

void GameManager::encodeDataTo(DS_Dictionary* dict)
{
    dict->setDictForKey("valueKeeper", m_valueKeeper);
    dict->setDictForKey("unlockValueKeeper", m_unlockValueKeeper);
    dict->setDictForKey("customObjectDict", m_customObjectDict);
    
    dict->setFloatForKey("bgVolume", FMODAudioEngine::sharedEngine()->getBackgroundMusicVolume());
    dict->setFloatForKey("sfxVolume", FMODAudioEngine::sharedEngine()->getEffectsVolume());
    dict->setIntegerForKey("timeOffset", (int)FMODAudioEngine::sharedEngine()->m_musicOffset);
    
    dict->setStringForKey("playerUDID", m_playerUDID);
    dict->setStringForKey("playerName", m_playerName);
    dict->setIntegerForKey("playerUserID", m_playerUserID.random - m_playerUserID.seed);
    dict->setIntegerForKey("playerFrame", m_playerFrame.random - m_playerFrame.seed);
    dict->setIntegerForKey("playerShip", m_playerShip.random - m_playerShip.seed);
    dict->setIntegerForKey("playerBall", m_playerBall.random - m_playerBall.seed);
    dict->setIntegerForKey("playerBird", m_playerBird.random - m_playerBird.seed);
    dict->setIntegerForKey("playerDart", m_playerDart.random - m_playerDart.seed);
    dict->setIntegerForKey("playerRobot", m_playerRobot.random - m_playerRobot.seed);
    dict->setIntegerForKey("playerSpider", m_playerSpider.random - m_playerSpider.seed);
    dict->setIntegerForKey("playerSwing", m_playerSwing.random - m_playerSwing.seed);
    dict->setIntegerForKey("playerColor", m_playerColor.random - m_playerColor.seed);
    dict->setIntegerForKey("playerColor2", m_playerColor2.random - m_playerColor2.seed);
    dict->setIntegerForKey("playerColor3", m_playerGlowColor.random - m_playerGlowColor.seed);
    dict->setIntegerForKey("playerStreak", m_playerStreak.random - m_playerStreak.seed);
    dict->setIntegerForKey("playerShipStreak", m_playerShipFire.random - m_playerShipFire.seed);
    dict->setIntegerForKey("playerDeathEffect", m_playerDeathEffect.random - m_playerDeathEffect.seed);
    dict->setIntegerForKey("playerJetpack", m_playerJetpack.random - m_playerJetpack.seed);
    dict->setIntegerForKey("playerIconType", m_playerIconType);
    dict->setBoolForKey("playerGlow", m_playerGlow);
    dict->setIntegerForKey("secretNumber", m_secretNumber.seed - m_secretNumber.random);
    dict->setIntegerForKey("hasRP", m_hasRP.random - m_hasRP.seed);
    dict->setBoolForKey("hasDRP", m_hasDRP);
    
    AchievementManager::sharedState()->encodeDataTo(dict);
    GameLevelManager::sharedState()->encodeDataTo(dict);
    GameStatsManager::sharedState()->encodeDataTo(dict);
    MusicDownloadManager::sharedState()->encodeDataTo(dict);
    GJAccountManager::sharedState()->encodeDataTo(dict);
    GJMultiplayerManager::sharedState()->encodeDataTo(dict);
    KeybindingsManager::sharedState()->encodeDataTo(dict);
    
    dict->setBoolForKey("showSongMarkers", m_showSongMarkers);
    dict->setBoolForKey("showBPMMarkers", m_showBPMMarkers);
    dict->setBoolForKey("recordGameplay", m_recordGameplay);
    dict->setBoolForKey("showProgressBar", m_showProgressBar);
    dict->setBoolForKey("performanceMode", m_performanceMode);
    dict->setBoolForKey("clickedGarage", m_clickedGarage);
    dict->setBoolForKey("clickedEditor", m_clickedEditor);
    dict->setBoolForKey("clickedName", m_clickedName);
    dict->setBoolForKey("clickedPractice", m_clickedPractice);
    dict->setBoolForKey("showedEditorGuide", m_showedEditorGuide);
    dict->setBoolForKey("showedRateDiffDialog", m_showedRateDiffDialog);
    dict->setBoolForKey("showedRateStarDialog", m_showedRateStarDialog);
    dict->setBoolForKey("showedLowDetailDialog", m_showedLowDetailDialog);
    dict->setIntegerForKey("bootups", m_bootups);
    dict->setBoolForKey("hasRatedGame", m_hasRatedGame);
    dict->setIntegerForKey("binaryVersion", 48);
    dict->setIntegerForKey("resolution", m_resolution);
    dict->setIntegerForKey("texQuality", m_texQuality);
    dict->setFloatForKey("customFPSTarget", m_customFPSTarget);
    dict->setIntegerForKey("customMenuSongID", m_customMenuSongID);
    dict->setIntegerForKey("customPracticeSongID", m_customPracticeSongID);
    
    dict->setStringForKey("dpad01", dpadConfigToString(&m_dpad1));
    dict->setStringForKey("dpad02", dpadConfigToString(&m_dpad2));
    dict->setStringForKey("dpad03", dpadConfigToString(&m_dpad3));
    dict->setStringForKey("dpad04", dpadConfigToString(&m_dpad4));
    dict->setStringForKey("dpad05", dpadConfigToString(&m_dpad5));
    
    dict->setStringForKey("dpadLayout01", m_dpadLayout1);
    dict->setStringForKey("dpadLayout02", m_dpadLayout2);
    dict->setStringForKey("dpadLayout03", m_dpadLayout3);
    dict->setStringForKey("dpadLayoutDual01", m_dpadLayoutDual1);
    dict->setStringForKey("dpadLayoutDual02", m_dpadLayoutDual2);
    dict->setStringForKey("dpadLayoutDual03", m_dpadLayoutDual3);
    
    dict->setFloatForKey("practiceOpacity", m_practiceOpacity);
    dict->setFloatForKey("practicePosX", m_practicePos.x);
    dict->setFloatForKey("practicePosY", m_practicePos.y);
}

void GameManager::doQuickSave()
{
    m_quickSave = true;
    GManager::save(this);
    m_quickSave = false;
}

bool GameManager::getGameVariable(const char* key)
{
    std::string dictKey = "gv_" + std::string(key);
    cocos2d::CCString* value = (cocos2d::CCString*)m_valueKeeper->valueForKey(dictKey);
    return value ? value->boolValue() : false;
}

void GameManager::setGameVariable(const char* key, bool value)
{
    std::string dictKey = "gv_" + std::string(key);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", value);
    m_valueKeeper->setObject(str, dictKey);
    
    if (std::string(key) == "0023")
    {
        m_unkBool8 = false;
    }
}

int GameManager::getIntGameVariable(const char* key)
{
    std::string dictKey = "gv_" + std::string(key);
    cocos2d::CCString* value = (cocos2d::CCString*)m_valueKeeper->valueForKey(dictKey);
    return value ? value->intValue() : 0;
}

void GameManager::setIntGameVariable(const char* key, int value)
{
    std::string dictKey = "gv_" + std::string(key);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", value);
    m_valueKeeper->setObject(str, dictKey);
}

bool GameManager::getUGV(const char* key)
{
    std::string dictKey = "ugv_" + std::string(key);
    cocos2d::CCString* value = (cocos2d::CCString*)m_unlockValueKeeper->valueForKey(dictKey);
    return value ? value->boolValue() : false;
}

void GameManager::setUGV(const char* key, bool value)
{
    std::string dictKey = "ugv_" + std::string(key);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", value);
    m_unlockValueKeeper->setObject(str, dictKey);
}

bool GameManager::toggleGameVariable(const char* key)
{
    bool current = getGameVariable(key);
    setGameVariable(key, !current);
    return !current;
}

void GameManager::accountStatusChanged()
{
    if (m_menuLayer)
    {
        m_menuLayer->updateUserProfileButton();
    }
}

int GameManager::activeIconForType(IconType type)
{
    int returnVal = 0;
    switch (type)
    {
        case Cube:
            returnVal = m_playerFrame.random - m_playerFrame.seed;
            break;
        case Ship:
            returnVal = m_playerShip.random - m_playerShip.seed;
            break;
        case Ball:
            returnVal = m_playerBall.random - m_playerBall.seed;
            break;
        case Ufo:
            returnVal = m_playerBird.random - m_playerBird.seed;
            break;
        case Wave:
            returnVal = m_playerDart.random - m_playerDart.seed;
            break;
        case Robot:
            returnVal = m_playerRobot.random - m_playerRobot.seed;
            break;
        case Spider:
            returnVal = m_playerSpider.random - m_playerSpider.seed;
            break;
        case Swing:
            returnVal = m_playerSwing.random - m_playerSwing.seed;
            break;
        case Jetpack:
            returnVal = m_playerJetpack.random - m_playerJetpack.seed;
            break;
        case DeathEffect:
            returnVal = m_playerDeathEffect.random - m_playerDeathEffect.seed;
            break;
        case Special:
            returnVal = m_playerStreak.random - m_playerStreak.seed;
            break;
        case ShipFire:
            returnVal = m_playerShipFire.random - m_playerShipFire.seed;
            break;
        default:
            returnVal = 0;
            break;
    }
    return returnVal;
}

void GameManager::checkUsedIcons()
{
    if (!isColorUnlocked(m_playerColor.random - m_playerColor.seed, Col1))
    {
        m_playerColor.value = 0;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerColor.seed = seed;
        m_playerColor.random = seed;
        
        if (!isColorUnlocked(m_playerColor2.random - m_playerColor2.seed, Col2))
        {
            m_playerColor2.value = 3;
            int seed2 = (int)((float)rand() * 4.6566e-10f * 1000.0f);
            m_playerColor2.seed = seed2;
            m_playerColor2.random = seed2 + 3;
        }
    }
    else if (!isColorUnlocked(m_playerColor2.random - m_playerColor2.seed, Col2))
    {
        m_playerColor2.value = 3;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerColor2.seed = seed;
        m_playerColor2.random = seed + 3;
    }
    
    if (!isColorUnlocked(m_playerGlowColor.random - m_playerGlowColor.seed, Col2))
    {
        int color = m_playerColor2.random - m_playerColor2.seed;
        m_playerGlowColor.value = color;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerGlowColor.seed = seed;
        m_playerGlowColor.random = color + seed;
    }
    
    if (!isIconUnlocked(m_playerFrame.random - m_playerFrame.seed, Cube))
    {
        m_playerFrame.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerFrame.seed = seed;
        m_playerFrame.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerShip.random - m_playerShip.seed, Ship))
    {
        m_playerShip.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerShip.seed = seed;
        m_playerShip.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerJetpack.random - m_playerJetpack.seed, Jetpack))
    {
        m_playerJetpack.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerJetpack.seed = seed;
        m_playerJetpack.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerBall.random - m_playerBall.seed, Ball))
    {
        m_playerBall.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerBall.seed = seed;
        m_playerBall.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerBird.random - m_playerBird.seed, Ufo))
    {
        m_playerBird.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerBird.seed = seed;
        m_playerBird.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerDart.random - m_playerDart.seed, Wave))
    {
        m_playerDart.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerDart.seed = seed;
        m_playerDart.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerRobot.random - m_playerRobot.seed, Robot))
    {
        m_playerRobot.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerRobot.seed = seed;
        m_playerRobot.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerSpider.random - m_playerSpider.seed, Spider))
    {
        m_playerSpider.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerSpider.seed = seed;
        m_playerSpider.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerSwing.random - m_playerSwing.seed, Swing))
    {
        m_playerSwing.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerSwing.seed = seed;
        m_playerSwing.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerStreak.random - m_playerStreak.seed, Special))
    {
        m_playerStreak.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerStreak.seed = seed;
        m_playerStreak.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerShipFire.random - m_playerShipFire.seed, ShipFire))
    {
        m_playerShipFire.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerShipFire.seed = seed;
        m_playerShipFire.random = seed + 1;
    }
    
    if (!isIconUnlocked(m_playerDeathEffect.random - m_playerDeathEffect.seed, DeathEffect))
    {
        m_playerDeathEffect.value = 1;
        int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
        m_playerDeathEffect.seed = seed;
        m_playerDeathEffect.random = seed + 1;
    }
    
    if (!isIconUnlocked(2, Special))
    {
        m_playerGlow = false;
    }
}

void GameManager::resetAllIcons()
{
    m_playerColor.value = 0;
    m_playerColor.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerColor.random = m_playerColor.seed;
    
    m_playerColor2.value = 3;
    m_playerColor2.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerColor2.random = m_playerColor2.seed + 3;
    
    m_playerGlowColor.value = -1;
    m_playerGlowColor.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerGlowColor.random = m_playerGlowColor.seed - 1;
    
    m_playerFrame.value = 1;
    m_playerFrame.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerFrame.random = m_playerFrame.seed + 1;
    
    m_playerShip.value = 1;
    m_playerShip.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerShip.random = m_playerShip.seed + 1;
    
    m_playerBall.value = 1;
    m_playerBall.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerBall.random = m_playerBall.seed + 1;
    
    m_playerBird.value = 1;
    m_playerBird.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerBird.random = m_playerBird.seed + 1;
    
    m_playerDart.value = 1;
    m_playerDart.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerDart.random = m_playerDart.seed + 1;
    
    m_playerRobot.value = 1;
    m_playerRobot.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerRobot.random = m_playerRobot.seed + 1;
    
    m_playerSpider.value = 1;
    m_playerSpider.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerSpider.random = m_playerSpider.seed + 1;
    
    m_playerSwing.value = 1;
    m_playerSwing.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerSwing.random = m_playerSwing.seed + 1;
    
    m_playerStreak.value = 1;
    m_playerStreak.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerStreak.random = m_playerStreak.seed + 1;
    
    m_playerShipFire.value = 1;
    m_playerShipFire.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerShipFire.random = m_playerShipFire.seed + 1;
    
    m_playerDeathEffect.value = 1;
    m_playerDeathEffect.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerDeathEffect.random = m_playerDeathEffect.seed + 1;
    
    m_playerJetpack.value = 1;
    m_playerJetpack.seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_playerJetpack.random = m_playerJetpack.seed + 1;
    
    m_playerGlow = false;
    m_playerIconType = Cube;
}

void GameManager::resetCoinUnlocks()
{
    std::string achievements[] = {
        "geometry.ach.coins01", "geometry.ach.coins02", "geometry.ach.coins03",
        "geometry.ach.coins04", "geometry.ach.coins05", "geometry.ach.coins06",
        "geometry.ach.coins07", "geometry.ach.coins08", "geometry.ach.coins09",
        "geometry.ach.coins10", "geometry.ach.coins11", "geometry.ach.coins12",
        "geometry.ach.coins13", "geometry.ach.coins14", "geometry.ach.coins15",
        "geometry.ach.coins16", "geometry.ach.coins17", "geometry.ach.coins18",
        "geometry.ach.coins19", "geometry.ach.coins20", "geometry.ach.coins21",
        "geometry.ach.coins22", "geometry.ach.coins23", "geometry.ach.coins24"
    };
    
    for (const auto& ach : achievements)
    {
        resetAchievement(ach);
    }
    
    checkUsedIcons();
}

void GameManager::resetDPadSettings(bool dual)
{
    if (dual)
    {
        m_dpad2.m_width = 280;
        m_dpad2.m_height = 120;
        m_dpad2.m_opacity = 255;
        m_dpad2.m_modeB = false;
        m_dpad2.m_deadzone = 0.0f;
        m_dpad2.m_scale = 1.0f;
        m_dpad2.m_radius = 10.0f;
        m_dpad2.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad2.m_oneButton = false;
        m_dpad2.m_player2 = false;
        m_dpad2.m_snap = false;
        m_dpad2.m_split = false;
        
        m_dpad3.m_width = 280;
        m_dpad3.m_height = 120;
        m_dpad3.m_opacity = 255;
        m_dpad3.m_modeB = false;
        m_dpad3.m_deadzone = 0.0f;
        m_dpad3.m_scale = 1.0f;
        m_dpad3.m_radius = 10.0f;
        m_dpad3.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad3.m_oneButton = false;
        m_dpad3.m_player2 = false;
        m_dpad3.m_snap = false;
        m_dpad3.m_split = false;
        
        m_dpad4.m_width = 280;
        m_dpad4.m_height = 120;
        m_dpad4.m_opacity = 255;
        m_dpad4.m_modeB = false;
        m_dpad4.m_deadzone = 0.0f;
        m_dpad4.m_scale = 1.0f;
        m_dpad4.m_radius = 10.0f;
        m_dpad4.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad4.m_oneButton = false;
        m_dpad4.m_player2 = false;
        m_dpad4.m_snap = false;
        m_dpad4.m_split = false;
        m_dpad4.m_width = 200;
        m_dpad4.m_height = 200;
        m_dpad4.m_position = cocos2d::CCPoint(95.0f, 36.0f + 60.0f + 100.0f);
        m_dpad4.m_oneButton = true;
        
        m_dpad5.m_width = 280;
        m_dpad5.m_height = 120;
        m_dpad5.m_opacity = 255;
        m_dpad5.m_modeB = false;
        m_dpad5.m_deadzone = 0.0f;
        m_dpad5.m_scale = 1.0f;
        m_dpad5.m_radius = 10.0f;
        m_dpad5.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad5.m_oneButton = false;
        m_dpad5.m_player2 = false;
        m_dpad5.m_snap = false;
        m_dpad5.m_split = false;
        m_dpad5.m_width = 200;
        m_dpad5.m_height = 200;
        m_dpad5.m_position = cocos2d::CCPoint(95.0f, 36.0f + 60.0f + 100.0f);
        m_dpad5.m_oneButton = true;
    }
    else
    {
        m_dpad1.m_width = 280;
        m_dpad1.m_height = 120;
        m_dpad1.m_opacity = 255;
        m_dpad1.m_modeB = false;
        m_dpad1.m_deadzone = 0.0f;
        m_dpad1.m_scale = 1.0f;
        m_dpad1.m_radius = 10.0f;
        m_dpad1.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad1.m_oneButton = false;
        m_dpad1.m_player2 = false;
        m_dpad1.m_snap = false;
        m_dpad1.m_split = false;
    }
}

void GameManager::prepareDPadSettings()
{
    if (m_dpad1.m_scale <= 0.0f || m_dpad2.m_scale <= 0.0f)
    {
        m_dpad2.m_width = 280;
        m_dpad2.m_height = 120;
        m_dpad2.m_deadzone = 0.0f;
        m_dpad2.m_modeB = false;
        m_dpad2.m_opacity = 255;
        m_dpad2.m_scale = 1.0f;
        m_dpad2.m_radius = 10.0f;
        m_dpad2.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad2.m_oneButton = false;
        m_dpad2.m_player2 = false;
        m_dpad2.m_snap = false;
        m_dpad2.m_split = false;
    }
    
    if (m_dpad3.m_scale <= 0.0f)
    {
        m_dpad3.m_width = 280;
        m_dpad3.m_height = 120;
        m_dpad3.m_deadzone = 0.0f;
        m_dpad3.m_modeB = false;
        m_dpad3.m_opacity = 255;
        m_dpad3.m_scale = 1.0f;
        m_dpad3.m_radius = 10.0f;
        m_dpad3.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad3.m_oneButton = false;
        m_dpad3.m_player2 = false;
        m_dpad3.m_snap = false;
        m_dpad3.m_split = false;
    }
    
    if (m_dpad4.m_scale <= 0.0f)
    {
        m_dpad4.m_width = 280;
        m_dpad4.m_height = 120;
        m_dpad4.m_deadzone = 0.0f;
        m_dpad4.m_modeB = false;
        m_dpad4.m_opacity = 255;
        m_dpad4.m_scale = 1.0f;
        m_dpad4.m_radius = 10.0f;
        m_dpad4.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad4.m_oneButton = false;
        m_dpad4.m_player2 = false;
        m_dpad4.m_snap = false;
        m_dpad4.m_split = false;
        m_dpad4.m_width = 200;
        m_dpad4.m_height = 200;
        m_dpad4.m_position = cocos2d::CCPoint(95.0f, 36.0f + 60.0f + 100.0f);
        m_dpad4.m_oneButton = true;
    }
    
    if (m_dpad5.m_scale <= 0.0f)
    {
        m_dpad5.m_width = 280;
        m_dpad5.m_height = 120;
        m_dpad5.m_deadzone = 0.0f;
        m_dpad5.m_modeB = false;
        m_dpad5.m_opacity = 255;
        m_dpad5.m_scale = 1.0f;
        m_dpad5.m_radius = 10.0f;
        m_dpad5.m_position = cocos2d::CCPoint(95.0f, 36.0f);
        m_dpad5.m_oneButton = false;
        m_dpad5.m_player2 = false;
        m_dpad5.m_snap = false;
        m_dpad5.m_split = false;
        m_dpad5.m_width = 200;
        m_dpad5.m_height = 200;
        m_dpad5.m_position = cocos2d::CCPoint(95.0f, 36.0f + 60.0f + 100.0f);
        m_dpad5.m_oneButton = true;
    }
    
    m_dpad4.m_oneButton = true;
    m_dpad5.m_oneButton = true;
    m_dpad3.m_player2 = true;
    m_dpad5.m_player2 = true;
}

void GameManager::loadDpadFromString(UIButtonConfig* config, std::string str)
{
    std::vector<std::string> parts;
    GameToolbox::split(str, ",", parts);
    
    if (parts.size() >= 6)
    {
        config->m_width = atoi(parts[0].c_str());
        config->m_height = atoi(parts[1].c_str());
        config->m_scale = atof(parts[2].c_str());
        config->m_opacity = (int)atof(parts[3].c_str());
        config->m_position.x = (float)atof(parts[4].c_str());
        config->m_position.y = (float)atof(parts[5].c_str());
    }
    
    if (parts.size() >= 11)
    {
        config->m_modeB = atoi(parts[6].c_str()) != 0;
        config->m_deadzone = atof(parts[7].c_str());
        config->m_radius = atof(parts[8].c_str());
        config->m_snap = atoi(parts[9].c_str()) != 0;
        config->m_split = atoi(parts[10].c_str()) != 0;
    }
}

std::string GameManager::dpadConfigToString(UIButtonConfig* config)
{
    std::string result = std::to_string(config->m_width) + "," +
                        std::to_string(config->m_height) + "," +
                        std::to_string(config->m_scale) + "," +
                        std::to_string(config->m_opacity) + "," +
                        std::to_string(config->m_position.x) + "," +
                        std::to_string(config->m_position.y) + "," +
                        std::to_string(config->m_modeB) + "," +
                        std::to_string(config->m_deadzone) + "," +
                        std::to_string(config->m_radius) + "," +
                        std::to_string(config->m_snap) + "," +
                        std::to_string(config->m_split);
    return result;
}

void GameManager::loadDPadLayout(int index, bool dual)
{
    std::string* layout = nullptr;
    
    if (dual)
    {
        if (index == 1) layout = &m_dpadLayoutDual1;
        else if (index == 2) layout = &m_dpadLayoutDual2;
        else if (index == 3) layout = &m_dpadLayoutDual3;
    }
    else
    {
        if (index == 1) layout = &m_dpadLayout1;
        else if (index == 2) layout = &m_dpadLayout2;
        else if (index == 3) layout = &m_dpadLayout3;
    }
    
    if (!layout) return;
    
    std::vector<std::string> parts;
    GameToolbox::split(*layout, ";", parts);
    
    if (parts.size() > 0)
    {
        loadDpadFromString(&m_dpad1, parts[0]);
        if (parts.size() > 1)
        {
            loadDpadFromString(&m_dpad2, parts[1]);
            loadDpadFromString(&m_dpad3, parts[2]);
            loadDpadFromString(&m_dpad4, parts[3]);
            loadDpadFromString(&m_dpad5, parts[4]);
        }
        
        if (dual && parts.size() > 0)
        {
            bool hasMode = atoi(parts[parts.size() - 1].c_str()) != 0;
            setGameVariable("0113", hasMode);
        }
    }
    
    prepareDPadSettings();
}

void GameManager::saveDPadLayout(int index, bool dual)
{
    std::string result;
    if (dual)
    {
        result = dpadConfigToString(&m_dpad2) + ";" +
                dpadConfigToString(&m_dpad3) + ";" +
                dpadConfigToString(&m_dpad4) + ";" +
                dpadConfigToString(&m_dpad5);
        
        if (index == 1) m_dpadLayoutDual1 = result;
        else if (index == 2) m_dpadLayoutDual2 = result;
        else if (index == 3) m_dpadLayoutDual3 = result;
    }
    else
    {
        result = dpadConfigToString(&m_dpad1) + ";" +
                std::to_string(getGameVariable("0113"));
        
        if (index == 1) m_dpadLayout1 = result;
        else if (index == 2) m_dpadLayout2 = result;
        else if (index == 3) m_dpadLayout3 = result;
    }
}

void GameManager::loadBackground(int index)
{
    int bgID = index;
    if (bgID <= 0) bgID = 1;
    else if (bgID > 59) bgID = 59;
    
    if (bgID != m_loadedBgID)
    {
        if (m_loadedBgID > 0)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("game_bg_%02d_001.png", m_loadedBgID);
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
        }
        
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("game_bg_%02d_001.png", bgID);
        cocos2d::CCTextureCache::sharedTextureCache()->addImage(str->getCString());
        m_loadedBgID = bgID;
    }
}

void GameManager::loadBackgroundAsync(int index)
{
    int bgID = index;
    if (bgID <= 0) bgID = 1;
    else if (bgID > 59) bgID = 59;
    
    if (!m_loadingBG && bgID != m_loadedBgID)
    {
        m_loadingBG = (void*)true;
        
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("game_bg_%02d_001.png", bgID);
        cocos2d::CCTextureCache::sharedTextureCache()->addImageAsync(
            str->getCString(),
            this,
            cocos2d::callfuncO_selector(GameManager::finishedLoadingBGAsync),
            bgID,
            kCCTexture2DPixelFormat_RGBA8888
        );
    }
}

void GameManager::finishedLoadingBGAsync(cocos2d::CCObject* obj)
{
    m_loadingBG = nullptr;
    
    if (obj && m_gameLayer)
    {
        int tag = obj->getTag();
        m_gameLayer->asyncBGLoaded(tag);
    }
    
    if (m_loadedBgID > 0)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("game_bg_%02d_001.png", m_loadedBgID);
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
    }
    
    if (obj)
    {
        m_loadedBgID = obj->getTag();
    }
}

void GameManager::loadGround(int index)
{
    int groundID = index;
    if (groundID <= 0) groundID = 1;
    else if (groundID > 22) groundID = 22;
    
    if (groundID != m_loadedGroundID)
    {
        if (m_loadedGroundID > 0)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("groundSquare_%02d_001.png", m_loadedGroundID);
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
            
            if (groundHasSecondaryColor(m_loadedGroundID))
            {
                cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("groundSquare_%02d_2_001.png", m_loadedGroundID);
                cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str2->getCString());
            }
        }
        
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("groundSquare_%02d_001.png", groundID);
        cocos2d::CCTextureCache::sharedTextureCache()->addImage(str->getCString());
        
        if (groundHasSecondaryColor(groundID))
        {
            cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("groundSquare_%02d_2_001.png", groundID);
            cocos2d::CCTextureCache::sharedTextureCache()->addImage(str2->getCString());
        }
        
        m_loadedGroundID = groundID;
    }
}

bool GameManager::groundHasSecondaryColor(int index)
{
    return (index >= 8 && index <= 11);
}

void GameManager::loadFont(int index)
{
    int fontID = index;
    if (fontID > 59) fontID = 59;
    if (fontID < 0) fontID = 0;
    
    if (fontID != m_loadedFont)
    {
        if (m_loadedFont > 0)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("gjFont%02d.png", m_loadedFont);
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
        }
        
        if (fontID > 0)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("gjFont%02d.png", fontID);
            cocos2d::CCTextureCache::sharedTextureCache()->addImage(str->getCString());
        }
        
        m_loadedFont = fontID;
    }
}

const char* GameManager::getFontFile(int index)
{
    int fontID = index;
    if (fontID < 0) fontID = 0;
    else if (fontID > 59) fontID = 59;
    
    loadFont(fontID);
    
    if (fontID > 0)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("gjFont%02d.fnt", fontID);
        return str->getCString();
    }
    
    return "bigFont.fnt";
}

const char* GameManager::getFontTexture(int index)
{
    int fontID = index;
    if (fontID < 0) fontID = 0;
    else if (fontID > 59) fontID = 59;
    
    loadFont(fontID);
    
    if (fontID > 0)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("gjFont%02d.png", fontID);
        return str->getCString();
    }
    
    return "bigFont.png";
}

void GameManager::loadDeathEffect(int id)
{
    int effectID = id;
    if (effectID <= 0) effectID = 1;
    else if (effectID > 20) effectID = 20;
    
    if (effectID != m_loadedDeathEffect)
    {
        if (m_loadedDeathEffect > 1)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("PlayerExplosion_%02d.png", m_loadedDeathEffect - 1);
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
        }
        
        if (effectID > 1)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("PlayerExplosion_%02d.png", effectID - 1);
            cocos2d::CCTextureCache::sharedTextureCache()->addImage(str->getCString());
            
            cocos2d::CCString* plist = cocos2d::CCString::createWithFormat("PlayerExplosion_%02d.plist", effectID - 1);
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plist->getCString());
        }
        
        m_loadedDeathEffect = effectID;
    }
}

void GameManager::loadMiddleground(int index)
{
    int mgID = index;
    if (mgID > 3) mgID = 3;
    if (mgID < 0) mgID = 0;
    
    if (m_loadedMG != mgID)
    {
        if (m_loadedMG > 0)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("fg_%02d_001.png", m_loadedMG);
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
            
            cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("fg_%02d_2_001.png", m_loadedMG);
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str2->getCString());
        }
        
        if (mgID > 0)
        {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("fg_%02d_001.png", mgID);
            cocos2d::CCTextureCache::sharedTextureCache()->addImage(str->getCString());
            
            cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("fg_%02d_2_001.png", mgID);
            cocos2d::CCTextureCache::sharedTextureCache()->addImage(str2->getCString());
        }
        
        m_loadedMG = mgID;
    }
}

void GameManager::loadMiddlegroundAsync(int index)
{
    int mgID = index;
    if (mgID <= 0) mgID = 1;
    else if (mgID > 3) mgID = 3;
    
    if (!m_loadingG1 && mgID != m_loadedMG)
    {
        m_finishedLoadingMG1 = false;
        m_finishedLoadingMG2 = false;
        m_loadingG1 = (void*)true;
        
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("fg_%02d_001.png", mgID);
        cocos2d::CCTextureCache::sharedTextureCache()->addImageAsync(
            str->getCString(),
            this,
            cocos2d::callfuncO_selector(GameManager::finishedLoadingMGAsync1),
            mgID,
            kCCTexture2DPixelFormat_RGBA8888
        );
        
        cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("fg_%02d_2_001.png", mgID);
        cocos2d::CCTextureCache::sharedTextureCache()->addImageAsync(
            str2->getCString(),
            this,
            cocos2d::callfuncO_selector(GameManager::finishedLoadingMGAsync2),
            mgID,
            kCCTexture2DPixelFormat_RGBA8888
        );
    }
}

void GameManager::finishedLoadingMGAsync1(cocos2d::CCObject* obj)
{
    m_finishedLoadingMG1 = true;
    if (m_finishedLoadingMG2)
    {
        finishedLoadingMGAsync(obj->getTag());
    }
}

void GameManager::finishedLoadingMGAsync2(cocos2d::CCObject* obj)
{
    m_finishedLoadingMG2 = true;
    if (m_finishedLoadingMG1)
    {
        finishedLoadingMGAsync(obj->getTag());
    }
}

void GameManager::finishedLoadingMGAsync(int index)
{
    m_loadingG1 = nullptr;
    
    if (m_gameLayer)
    {
        m_gameLayer->asyncMGLoaded(index);
    }
    
    if (m_loadedMG > 0)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("fg_%02d_001.png", m_loadedMG);
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
        
        cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("fg_%02d_2_001.png", m_loadedMG);
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str2->getCString());
    }
    
    m_loadedMG = index;
}

bool GameManager::isIconUnlocked(int id, IconType type)
{
    if (type == Cube && id <= 4)
    {
        return true;
    }
    if (type != Cube && id <= 1)
    {
        return true;
    }
    
    std::string key = iconKey(id, type);
    cocos2d::CCString* value = (cocos2d::CCString*)m_valueKeeper->valueForKey(key);
    
    if (value && value->boolValue())
    {
        return true;
    }
    
    UnlockType unlockType = iconTypeToUnlockType(type);
    return GameStatsManager::sharedState()->isItemUnlocked(unlockType, id);
}

bool GameManager::isColorUnlocked(int id, UnlockType type)
{
    if (id <= 3)
    {
        return true;
    }
    
    std::string key = colorKey(id, type);
    cocos2d::CCString* value = (cocos2d::CCString*)m_valueKeeper->valueForKey(key);
    
    if (value && value->boolValue())
    {
        return true;
    }
    
    return GameStatsManager::sharedState()->isItemUnlocked(type, id);
}

void GameManager::unlockIcon(int id, IconType type)
{
    std::string key = iconKey(id, type);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", 1);
    m_valueKeeper->setObject(str, key);
}

void GameManager::unlockColor(int id, UnlockType type)
{
    std::string key = colorKey(id, type);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", 1);
    m_valueKeeper->setObject(str, key);
}

void GameManager::lockIcon(int id, IconType type)
{
    std::string key = iconKey(id, type);
    m_valueKeeper->removeObjectForKey(key);
}

void GameManager::lockColor(int id, UnlockType type)
{
    std::string key = colorKey(id, type);
    m_valueKeeper->removeObjectForKey(key);
}

std::string GameManager::iconKey(int id, IconType type)
{
    std::string prefix;
    switch (type)
    {
        case Cube: prefix = "i_"; break;
        case Ship: prefix = "ship_"; break;
        case Ball: prefix = "ball_"; break;
        case Ufo: prefix = "bird_"; break;
        case Wave: prefix = "dart_"; break;
        case Robot: prefix = "robot_"; break;
        case Spider: prefix = "spider_"; break;
        case Swing: prefix = "swing_"; break;
        case Jetpack: prefix = "jetpack_"; break;
        case DeathEffect: prefix = "death_"; break;
        case Special: prefix = "special_"; break;
        case ShipFire: prefix = "shipstreak_"; break;
        default: prefix = "i_"; break;
    }
    return prefix + std::to_string(id);
}

std::string GameManager::colorKey(int id, UnlockType type)
{
    std::string prefix = type == Col1 ? "c1" : "c2";
    return "c" + std::to_string(type == Col1 ? 1 : 2) + "o" + std::to_string(id);
}

std::string GameManager::sheetNameForIcon(int id, int type)
{
    std::string prefix;
    switch (type)
    {
        case Cube: prefix = "player_"; break;
        case Ship: prefix = "ship_"; break;
        case Ball: prefix = "player_ball_"; break;
        case Ufo: prefix = "bird_"; break;
        case Wave: prefix = "dart_"; break;
        case Robot: prefix = "robot_"; break;
        case Spider: prefix = "spider_"; break;
        case Swing: prefix = "swing_"; break;
        case Jetpack: prefix = "jetpack_"; break;
        default: return "";
    }
    
    std::string result = "icons/" + prefix;
    if (id <= 9)
    {
        result += "0";
    }
    result += std::to_string(id) + ".png";
    return result;
}

int GameManager::keyForIcon(int id, int type)
{
    return id + m_keyStartForIcon[type] - 1;
}

void GameManager::iconAndTypeForKey(int key, int* id, int* type)
{
    int* start = m_keyStartForIcon.data();
    
    if (key < start[1])
    {
        *type = 0;
        *id = key - start[0] + 1;
    }
    else if (key < start[2])
    {
        *type = 1;
        *id = key - start[1] + 1;
    }
    else if (key < start[3])
    {
        *type = 2;
        *id = key - start[2] + 1;
    }
    else if (key < start[4])
    {
        *type = 3;
        *id = key - start[3] + 1;
    }
    else if (key < start[5])
    {
        *type = 4;
        *id = key - start[4] + 1;
    }
    else if (key < start[6])
    {
        *type = 5;
        *id = key - start[5] + 1;
    }
    else if (key < start[7])
    {
        *type = 6;
        *id = key - start[6] + 1;
    }
    else
    {
        *type = 7;
        *id = key - start[7] + 1;
    }
}

void GameManager::calculateBaseKeyForIcons()
{
    if (m_keyStartForIcon.size() < 9)
    {
        m_keyStartForIcon.resize(9);
    }
    
    m_keyStartForIcon[0] = 0;
    m_keyStartForIcon[1] = 485;
    m_keyStartForIcon[2] = 654;
    m_keyStartForIcon[3] = 772;
    m_keyStartForIcon[4] = 921;
    m_keyStartForIcon[5] = 1017;
    m_keyStartForIcon[6] = 1085;
    m_keyStartForIcon[7] = 1154;
    m_keyStartForIcon[8] = 1197;
    
    for (int i = 0; i < 1205; i++)
    {
        m_iconLoadCounts[i] = 0;
    }
}

UnlockType GameManager::iconTypeToUnlockType(IconType type)
{
    switch (type)
    {
        case Cube: return CubeType;
        case Ship: return ShipType;
        case Ball: return BallType;
        case Ufo: return UfoType;
        case Wave: return WaveType;
        case Robot: return RobotType;
        case Spider: return SpiderType;
        case Swing: return SwingType;
        case Jetpack: return JetpackType;
        case DeathEffect: return DeathType;
        case Special: return StreakType;
        case ShipFire: return ShipFireType;
        default: return CubeType;
    }
}

IconType GameManager::unlockTypeToIconType(int type)
{
    switch (type)
    {
        case CubeType: return Cube;
        case ShipType: return Ship;
        case BallType: return Ball;
        case UfoType: return Ufo;
        case WaveType: return Wave;
        case RobotType: return Robot;
        case SpiderType: return Spider;
        case SwingType: return Swing;
        case JetpackType: return Jetpack;
        case DeathType: return DeathEffect;
        case StreakType: return Special;
        case ShipFireType: return ShipFire;
        default: return Cube;
    }
}

int GameManager::countForType(IconType type)
{
    switch (type)
    {
        case Cube: return 485;
        case Ship: return 169;
        case Ball: return 118;
        case Ufo: return 149;
        case Wave: return 96;
        case Robot: return 68;
        case Spider: return 69;
        case Swing: return 43;
        case Jetpack: return 8;
        case DeathEffect: return 20;
        case Special: return 7;
        case ShipFire: return 6;
        default: return 0;
    }
}

void GameManager::addIconDelegate(cocos2d::CCObject* delegate, int key)
{
    m_iconDelegates[key].push_back(delegate);
}

void GameManager::removeIconDelegate(int requestID)
{
    for (auto& pair : m_iconDelegates)
    {
        auto& vec = pair.second;
        vec.erase(std::remove_if(vec.begin(), vec.end(),
            [requestID](cocos2d::CCObject* obj) {
                return obj->getTag() == requestID;
            }), vec.end());
    }
}

int GameManager::getIconRequestID()
{
    return m_iconRequestID++;
}

bool GameManager::isIconLoaded(int id, int type)
{
    int key = keyForIcon(id, type);
    return m_iconLoadCounts[key] > 0;
}

void GameManager::finishedLoadingIconAsync(cocos2d::CCObject* obj)
{
    if (!obj) return;
    
    int key = obj->getTag();
    int id = 0, type = 0;
    iconAndTypeForKey(key, &id, &type);
    
    loadIcon(id, type, -1);
    
    m_isIconBeingLoaded[key] = false;
    
    for (auto& pair : m_iconDelegates)
    {
        if (pair.first == key)
        {
            for (auto delegate : pair.second)
            {
                SimplePlayer* player = dynamic_cast<SimplePlayer*>(delegate);
                if (player)
                {
                    player->iconFinishedLoading(id, (IconType)type);
                }
            }
        }
    }
}

cocos2d::CCTexture2D* GameManager::loadIcon(int id, int type, int requestID)
{
    std::string sheetName = sheetNameForIcon(id, type);
    if (sheetName.empty()) return nullptr;
    
    std::string pngPath = sheetName;
    int key = keyForIcon(id, type);
    
    m_iconLoadCounts[key]++;
    
    std::string plistPath = sheetName.substr(0, sheetName.find_last_of('.')) + ".plist";
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistPath.c_str());
    cocos2d::CCTexture2D* texture = cocos2d::CCTextureCache::sharedTextureCache()->addImage(pngPath.c_str());
    
    // Register request
    m_iconRequests[requestID][type] = id;
    
    return texture;
}

void GameManager::loadIconAsync(int id, int type, int requestID, cocos2d::CCObject* delegate)
{
    std::string sheetName = sheetNameForIcon(id, type);
    if (sheetName.empty()) return;
    
    int key = keyForIcon(id, type);
    addIconDelegate(delegate, key);
    
    if (m_isIconBeingLoaded[key])
    {
        return;
    }
    
    m_isIconBeingLoaded[key] = true;
    
    std::string pngPath = sheetName;
    cocos2d::CCTextureCache::sharedTextureCache()->addImageAsync(
        pngPath.c_str(),
        this,
        cocos2d::callfuncO_selector(GameManager::finishedLoadingIconAsync),
        key,
        kCCTexture2DPixelFormat_RGBA8888
    );
}

void GameManager::unloadIcon(int id, int type, int requestID)
{
    if (id <= 0) return;
    
    int key = keyForIcon(id, type);
    m_iconLoadCounts[key]--;
    
    if (m_iconLoadCounts[key] <= 0)
    {
        std::string sheetName = sheetNameForIcon(id, type);
        if (!sheetName.empty())
        {
            std::string pngPath = sheetName;
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(pngPath.c_str());
        }
    }
    
    // Clean requests
    if (m_iconRequests.count(requestID))
    {
        m_iconRequests[requestID].erase(type);
        if (m_iconRequests[requestID].empty())
        {
            m_iconRequests.erase(requestID);
        }
    }
}

void GameManager::unloadIcons(int requestID)
{
    if (m_iconRequests.count(requestID))
    {
        for (auto& pair : m_iconRequests[requestID])
        {
            unloadIcon(pair.second, (IconType)pair.first, requestID);
        }
        m_iconRequests.erase(requestID);
    }
}

void GameManager::unloadBackground()
{
    if (m_loadedBgID > 0)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("game_bg_%02d_001.png", m_loadedBgID);
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
        m_loadedBgID = 0;
    }
}

const char* GameManager::getBGTexture(int index)
{
    int bgID = index;
    if (bgID <= 0) bgID = 1;
    else if (bgID > 59) bgID = 59;
    
    loadBackground(bgID);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("game_bg_%02d_001.png", bgID);
    return str->getCString();
}

const char* GameManager::getGTexture(int index)
{
    int groundID = index;
    if (groundID <= 0) groundID = 1;
    else if (groundID > 22) groundID = 22;
    
    loadGround(groundID);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("groundSquare_%02d_001.png", groundID);
    return str->getCString();
}

const char* GameManager::getMGTexture(int index)
{
    int mgID = index;
    if (mgID <= 0) mgID = 1;
    else if (mgID > 3) mgID = 3;
    
    loadMiddleground(mgID);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("fg_%02d_001.png", mgID);
    return str->getCString();
}

void GameManager::loadGroundAsync(int index)
{
    int groundID = index;
    if (groundID <= 0) groundID = 1;
    else if (groundID > 22) groundID = 22;
    
    if (!m_loadingG && groundID != m_loadedGroundID)
    {
        m_loadingG = (void*)true;
        m_finishedLoadingG1 = false;
        m_finishedLoadingG2 = !groundHasSecondaryColor(groundID);
        
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("groundSquare_%02d_001.png", groundID);
        cocos2d::CCTextureCache::sharedTextureCache()->addImageAsync(
            str->getCString(),
            this,
            cocos2d::callfuncO_selector(GameManager::finishedLoadingGAsync1),
            groundID,
            kCCTexture2DPixelFormat_RGBA8888
        );
        
        if (groundHasSecondaryColor(groundID))
        {
            cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("groundSquare_%02d_2_001.png", groundID);
            cocos2d::CCTextureCache::sharedTextureCache()->addImageAsync(
                str2->getCString(),
                this,
                cocos2d::callfuncO_selector(GameManager::finishedLoadingGAsync2),
                groundID,
                kCCTexture2DPixelFormat_RGBA8888
            );
        }
    }
}

void GameManager::finishedLoadingGAsync1(cocos2d::CCObject* obj)
{
    m_finishedLoadingG1 = true;
    if (m_finishedLoadingG2)
    {
        finishedLoadingGAsync(obj->getTag());
    }
}

void GameManager::finishedLoadingGAsync2(cocos2d::CCObject* obj)
{
    m_finishedLoadingG2 = true;
    if (m_finishedLoadingG1)
    {
        finishedLoadingGAsync(obj->getTag());
    }
}

void GameManager::finishedLoadingGAsync(int index)
{
    m_loadingG = nullptr;
    
    if (m_gameLayer)
    {
        m_gameLayer->asyncGLoaded(index);
    }
    
    if (m_loadedGroundID > 0)
    {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("groundSquare_%02d_001.png", m_loadedGroundID);
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str->getCString());
        
        if (groundHasSecondaryColor(m_loadedGroundID))
        {
            cocos2d::CCString* str2 = cocos2d::CCString::createWithFormat("groundSquare_%02d_2_001.png", m_loadedGroundID);
            cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(str2->getCString());
        }
    }
    
    m_loadedGroundID = index;
}

void GameManager::logLoadedIconInfo()
{
    for (int i = 0; i < 1205; i++)
    {
        if (m_iconLoadCounts[i] > 0)
        {
            // Log icon loaded
        }
    }
}

void GameManager::setupGameAnimations()
{
    if (!m_mainFramesForAnimation)
    {
        m_mainFramesForAnimation = cocos2d::CCDictionary::create();
        m_mainFramesForAnimation->retain();
        
        m_detailFramesForAnimation = cocos2d::CCDictionary::create();
        m_detailFramesForAnimation->retain();
        
        m_frameTimeForAnimation = cocos2d::CCDictionary::create();
        m_frameTimeForAnimation->retain();
        
        m_framesForAnimation = cocos2d::CCDictionary::create();
        m_framesForAnimation->retain();
        
        m_defaultFrames = cocos2d::CCDictionary::create();
        m_defaultFrames->retain();
        
        // All animations
        addGameAnimation(1050, 10, 0.05, "d_animWave_01", "", 1);
        addGameAnimation(1051, 10, 0.05, "d_animWave_02", "", 1);
        addGameAnimation(1052, 10, 0.05, "d_animWave_03", "", 1);
        addGameAnimation(3000, 10, 0.05, "d_animWave_01b", "d_animWave_01b_color", 1);
        addGameAnimation(3001, 10, 0.05, "d_animWave_02b", "d_animWave_02b_color", 1);
        addGameAnimation(3002, 10, 0.05, "d_animWave_03b", "d_animWave_03b_color", 1);
        addGameAnimation(1592, 8, 0.045, "d_animSquare_01", "d_animSquare_01", 1);
        addGameAnimation(1053, 6, 0.045, "d_animLoading_01", "d_animLoading_01_color", 1);
        addGameAnimation(1054, 7, 0.06, "d_animLoading_02", "d_animLoading_02_color", 1);
        addGameAnimation(1614, 4, 0.12, "smallCoin_01", "smallCoin_01_color", 1);
        addGameAnimation(1516, 10, 0.05, "waterfallAnim", "waterfallAnim_color", 1);
        addGameAnimation(1518, 6, 0.05, "waterSplash", "", 1);
        addGameAnimation(923, 9, 0.08, "Fire_01_looped", "Fire_01_2_looped", 1);
        addGameAnimation(924, 9, 0.08, "Fire_02_looped", "Fire_02_2_looped", 1);
        addGameAnimation(920, 9, 0.08, "Fire_03_looped", "Fire_03_2_looped", 1);
        addGameAnimation(921, 11, 0.08, "Fire_04_looped", "Fire_04_2_looped", 1);
        addGameAnimation(1936, 13, 0.07, "fire_b_01", "fire_b_01_2", 1);
        addGameAnimation(1937, 8, 0.07, "fire_b_02", "fire_b_02_2", 1);
        addGameAnimation(1938, 8, 0.07, "fire_b_03", "fire_b_03_2", 1);
        addGameAnimation(1939, 8, 0.07, "fire_b_04", "fire_b_04_2", 1);
        addGameAnimation(1583, 4, 0.08, "fireball_02", "fireball_02_color", 1);
        addGameAnimation(2058, 8, 0.13, "lava_top", "lava_top_color", 1);
        addGameAnimation(2059, 8, 0.12, "lava_top_bubble", "lava_top_bubble_color", 1);
        addGameAnimation(2060, 6, 0.12, "lava_top_bubble02", "lava_top_bubble02_color", 1);
        addGameAnimation(1519, 11, 0.06, "starAnim", "", 1);
        addGameAnimation(1618, 7, 0.05, "explosion_01", "", 1);
        addGameAnimation(1697, 3, 0.06, "d_zag_01", "d_zag_01_color", 1);
        addGameAnimation(1698, 2, 0.06, "d_zag_02", "d_zag_02_color", 1);
        addGameAnimation(1699, 2, 0.06, "d_zag_03", "d_zag_03_color", 1);
        addGameAnimation(1849, 6, 0.07, "gj_smoke01", "gj_smoke01_color", 1);
        addGameAnimation(1850, 5, 0.1, "gj_smoke02", "gj_smoke02_color", 1);
        addGameAnimation(1851, 8, 0.07, "gj_drops01", "gj_drops01_color", 1);
        addGameAnimation(1852, 8, 0.07, "gj_drops02", "gj_drops02_color", 1);
        addGameAnimation(1853, 8, 0.08, "gj_drops03", "gj_drops03_color", 1);
        addGameAnimation(1854, 14, 0.07, "gj_drops04", "gj_drops04_color", 1);
        addGameAnimation(1855, 16, 0.06, "gj_drops05", "gj_drops05_color", 1);
        addGameAnimation(1858, 8, 0.08, "gj_drops06", "", 1);
        addGameAnimation(1856, 14, 0.07, "gj_bubble01", "gj_bubble01_color", 1);
        addGameAnimation(1857, 3, 0.06, "gj_lightning01", "gj_lightning01_color", 1);
        addGameAnimation(1860, 11, 0.05, "gj_lightning02", "gj_lightning02_color", 1);
        addGameAnimation(2020, 6, 0.05, "gj22_anim_01", "gj22_anim_01_color", 1);
        addGameAnimation(2021, 6, 0.05, "gj22_anim_02", "gj22_anim_02_color", 1);
        addGameAnimation(2022, 6, 0.05, "gj22_anim_03", "gj22_anim_03_color", 1);
        addGameAnimation(2023, 3, 0.06, "gj22_anim_04", "gj22_anim_04_color", 1);
        addGameAnimation(2024, 5, 0.05, "gj22_anim_05", "gj22_anim_05_color", 1);
        addGameAnimation(2025, 12, 0.05, "gj22_anim_06", "gj22_anim_06_color", 1);
        addGameAnimation(2026, 6, 0.05, "gj22_anim_07", "gj22_anim_07_color", 1);
        addGameAnimation(2027, 9, 0.05, "gj22_anim_08", "gj22_anim_08_color", 1);
        addGameAnimation(2028, 5, 0.05, "gj22_anim_09", "gj22_anim_09_color", 1);
        addGameAnimation(2029, 11, 0.05, "gj22_anim_10", "gj22_anim_10_color", 1);
        addGameAnimation(2030, 7, 0.05, "gj22_anim_11", "gj22_anim_11_color", 1);
        addGameAnimation(2031, 6, 0.05, "gj22_anim_12", "gj22_anim_12_color", 1);
        addGameAnimation(2032, 8, 0.05, "gj22_anim_13", "gj22_anim_13_color", 1);
        addGameAnimation(2033, 8, 0.05, "gj22_anim_14", "gj22_anim_14_color", 1);
        addGameAnimation(2034, 5, 0.08, "gj22_anim_15", "gj22_anim_15_color", 1);
        addGameAnimation(2035, 6, 0.05, "gj22_anim_16", "gj22_anim_16_color", 1);
        addGameAnimation(2036, 5, 0.05, "gj22_anim_17", "gj22_anim_17_color", 1);
        addGameAnimation(2037, 9, 0.05, "gj22_anim_18", "gj22_anim_18_color", 1);
        addGameAnimation(2038, 4, 0.05, "gj22_anim_19", "gj22_anim_19_color", 1);
        addGameAnimation(2039, 8, 0.05, "gj22_anim_20", "gj22_anim_20_color", 1);
        addGameAnimation(2040, 6, 0.05, "gj22_anim_21", "gj22_anim_21_color", 1);
        addGameAnimation(2041, 12, 0.05, "gj22_anim_22", "", 1);
        addGameAnimation(2042, 7, 0.05, "gj22_anim_23", "", 1);
        addGameAnimation(2043, 9, 0.05, "gj22_anim_24", "gj22_anim_24_color", 1);
        addGameAnimation(2044, 5, 0.05, "gj22_anim_25", "gj22_anim_25_color", 1);
        addGameAnimation(2045, 12, 0.05, "gj22_anim_26", "gj22_anim_26_color", 1);
        addGameAnimation(2046, 9, 0.05, "gj22_anim_27", "gj22_anim_27_color", 1);
        addGameAnimation(2047, 11, 0.04, "gj22_anim_28", "gj22_anim_28_color", 1);
        addGameAnimation(2048, 3, 0.05, "gj22_anim_29", "gj22_anim_29_color", 1);
        addGameAnimation(2049, 3, 0.05, "gj22_anim_30", "gj22_anim_30_color", 1);
        addGameAnimation(2050, 4, 0.05, "gj22_anim_31", "gj22_anim_31_color", 1);
        addGameAnimation(2051, 9, 0.05, "gj22_anim_32", "gj22_anim_32_color", 1);
        addGameAnimation(2052, 7, 0.05, "gj22_anim_33", "gj22_anim_33_color", 1);
        addGameAnimation(2053, 7, 0.05, "gj22_anim_34", "gj22_anim_34_color", 1);
        addGameAnimation(2054, 8, 0.05, "gj22_anim_35", "gj22_anim_35_color", 1);
        addGameAnimation(2055, 9, 0.05, "gj22_anim_36", "gj22_anim_36_color", 1);
        addGameAnimation(2864, 8, 0.06, "gj22_anim_37", "gj22_anim_37_color", 1);
        addGameAnimation(2865, 8, 0.06, "gj22_anim_38", "gj22_anim_38_color", 1);
        addGameAnimation(2867, 12, 0.06, "gj22_anim_39", "gj22_anim_39_color", 1);
        addGameAnimation(2868, 3, 0.06, "gj22_anim_39b", "", 1);
        addGameAnimation(2869, 13, 0.05, "gj22_anim_40", "gj22_anim_40_color", 1);
        addGameAnimation(2870, 12, 0.05, "gj22_anim_40b", "gj22_anim_40b_color", 1);
        addGameAnimation(2871, 8, 0.05, "gj22_anim_41", "gj22_anim_41_color", 1);
        addGameAnimation(2872, 12, 0.05, "gj22_anim_42", "gj22_anim_42_color", 1);
        addGameAnimation(2873, 5, 0.05, "gj22_anim_43", "gj22_anim_43_color", 1);
        addGameAnimation(2874, 4, 0.05, "gj22_anim_44", "gj22_anim_44_color", 1);
        addGameAnimation(2875, 7, 0.05, "gj22_anim_45", "gj22_anim_45_color", 1);
        addGameAnimation(2876, 8, 0.05, "gj22_anim_46", "gj22_anim_46_color", 1);
        addGameAnimation(2877, 10, 0.06, "gj22_anim_47", "gj22_anim_47_color", 1);
        addGameAnimation(2878, 12, 0.06, "gj22_anim_48", "gj22_anim_48_color", 1);
        addGameAnimation(2879, 4, 0.06, "gj22_anim_49", "gj22_anim_49_color", 1);
        addGameAnimation(2880, 12, 0.05, "gj22_anim_50", "gj22_anim_50_color", 1);
        addGameAnimation(2881, 9, 0.05, "gj22_anim_51", "gj22_anim_51_color", 1);
        addGameAnimation(2882, 7, 0.05, "gj22_anim_52", "gj22_anim_52_color", 1);
        addGameAnimation(2883, 7, 0.05, "gj22_anim_53", "gj22_anim_53_color", 1);
        addGameAnimation(2884, 6, 0.05, "gj22_anim_54", "gj22_anim_54_color", 1);
        addGameAnimation(2885, 8, 0.05, "gj22_anim_55", "gj22_anim_55_color", 1);
        addGameAnimation(2886, 5, 0.05, "gj22_anim_56", "gj22_anim_56_color", 1);
        addGameAnimation(2887, 5, 0.06, "gj22_anim_57", "gj22_anim_57_color", 1);
        addGameAnimation(2888, 6, 0.06, "gj22_anim_58", "gj22_anim_58_color", 1);
        addGameAnimation(2889, 4, 0.06, "gj22_anim_59", "gj22_anim_59_color", 1);
        addGameAnimation(2890, 8, 0.05, "gj22_anim_60", "gj22_anim_60_color", 1);
        addGameAnimation(2891, 4, 0.05, "gj22_anim_61", "gj22_anim_61_color", 1);
        addGameAnimation(2892, 2, 0.05, "gj22_anim_62", "gj22_anim_62_color", 1);
        addGameAnimation(2893, 4, 0.05, "gj22_anim_63", "gj22_anim_63_color", 1);
        addGameAnimation(2894, 8, 0.05, "gj22_anim_64", "gj22_anim_64_color", 1);
        addGameAnimation(2223, 4, 0.1, "pixelart_123", "pixelart_123_color", 1);
        addGameAnimation(2246, 8, 0.1, "pixelart_146", "pixelart_146_color", 1);
        addGameAnimation(2605, 8, 0.1, "pixelart_505", "pixelart_505_color", 1);
        addGameAnimation(2629, 8, 0.1, "pixelart_529", "pixelart_529_color", 1);
        addGameAnimation(2630, 4, 0.1, "pixelart_530", "pixelart_530_color", 1);
        addGameAnimation(2694, 5, 0.16, "pixelart_594", "pixelart_594_color", 1);
        addGameAnimation(3119, 5, 0.16, "pixelart_619", "pixelart_619_color", 1);
        addGameAnimation(3120, 12, 0.16, "pixelart_620", "pixelart_620_color", 1);
        addGameAnimation(3121, 10, 0.16, "pixelart_621", "pixelart_621_color", 1);
        addGameAnimation(3219, 8, 0.16, "pixelart_719", "pixelart_719_color", 1);
        addGameAnimation(3303, 4, 0.12, "pixelart_803", "pixelart_803_color", 1);
        addGameAnimation(3304, 5, 0.12, "pixelart_804", "pixelart_804_color", 1);
        addGameAnimation(3482, 6, 0.12, "pixelart_982", "pixelart_982_color", 1);
        addGameAnimation(3483, 6, 0.12, "pixelart_983", "pixelart_983_color", 1);
        addGameAnimation(3484, 6, 0.12, "pixelart_984", "pixelart_984_color", 1);
        addGameAnimation(3492, 4, 0.12, "pixelart_992", "pixelart_992_color", 1);
        addGameAnimation(3493, 4, 0.12, "pixelart_993", "pixelart_993_color", 1);
        addGameAnimation(4211, 4, 0.12, "pixelart_1411", "pixelart_1411_color", 1);
        addGameAnimation(142, 4, 0.115, "secretCoin_01", "secretCoin_01_color", 1);
        addGameAnimation(1329, 4, 0.115, "secretCoin_2_01", "secretCoin_2_01_color", 1);
        addGameAnimation(-142, 4, 0.115, "secretCoin_b_01", "secretCoin_b_01_color", 1);
        addGameAnimation(-1329, 4, 0.115, "secretCoin_2_b_01", "secretCoin_2_b_01_color", 1);
        addGameAnimation(4300, 4, 0.08, "pixelart_1500", "pixelart_1500_color", 1);
        addGameAnimation(1591, 1, 1.0, "lava_top", "", 1);
        addGameAnimation(1593, 1, 1.0, "lava_top", "", 1);
        addGameAnimation(1839, 1, 1.0, "lava_top", "", 1);
        addGameAnimation(1840, 1, 1.0, "lava_top", "", 1);
        addGameAnimation(1841, 1, 1.0, "lava_top", "", 1);
        addGameAnimation(1842, 1, 1.0, "lava_top", "", 1);
        addGameAnimation(2063, 1, 1.0, "lava_top", "", 1);
    }
}

void GameManager::addGameAnimation(int objectID, int frames, float frameTime, const std::string& mainFrame, const std::string& detailFrame, int defaultFrame)
{
    cocos2d::CCArray* mainArray = cocos2d::CCArray::create();
    cocos2d::CCArray* detailArray = cocos2d::CCArray::create();
    
    for (int i = 0; i < frames; i++)
    {
        mainArray->addObject(cocos2d::CCString::createWithFormat("%s_%02d.png", mainFrame.c_str(), i + 1));
        if (!detailFrame.empty())
        {
            detailArray->addObject(cocos2d::CCString::createWithFormat("%s_%02d.png", detailFrame.c_str(), i + 1));
        }
    }
    
    std::string key = std::to_string(objectID);
    m_mainFramesForAnimation->setObject(mainArray, key);
    if (detailArray->count() > 0)
    {
        m_detailFramesForAnimation->setObject(detailArray, key);
    }
    m_frameTimeForAnimation->setObject(cocos2d::CCString::createWithFormat("%f", frameTime), key);
    m_framesForAnimation->setObject(cocos2d::CCString::createWithFormat("%i", frames), key);
    m_defaultFrames->setObject(cocos2d::CCString::createWithFormat("%i", defaultFrame), key);
}

void GameManager::addCustomAnimationFrame(int objectID, int frame, const std::string& mainFrame, const std::string& detailFrame)
{
    std::string key = std::to_string(objectID);
    
    cocos2d::CCArray* mainArray = (cocos2d::CCArray*)m_mainFramesForAnimation->objectForKey(key);
    if (mainArray)
    {
        mainArray->addObject(cocos2d::CCString::create(mainFrame));
    }
    
    if (!detailFrame.empty())
    {
        cocos2d::CCArray* detailArray = (cocos2d::CCArray*)m_detailFramesForAnimation->objectForKey(key);
        if (detailArray)
        {
            detailArray->addObject(cocos2d::CCString::create(detailFrame));
        }
    }
}

int GameManager::framesForAnimation(int objectID)
{
    std::string key = std::to_string(objectID);
    cocos2d::CCString* value = (cocos2d::CCString*)m_framesForAnimation->objectForKey(key);
    return value ? value->intValue() : 1;
}

float GameManager::frameTimeForAnimation(int objectID)
{
    std::string key = std::to_string(objectID);
    cocos2d::CCString* value = (cocos2d::CCString*)m_frameTimeForAnimation->objectForKey(key);
    return value ? value->floatValue() : 1.0f;
}

int GameManager::defaultFrameForAnimation(int objectID)
{
    std::string key = std::to_string(objectID);
    cocos2d::CCString* value = (cocos2d::CCString*)m_defaultFrames->objectForKey(key);
    return value ? value->intValue() : 1;
}

void GameManager::addDuplicateLastFrame(int objectID)
{
    std::string key = std::to_string(objectID);
    int frames = framesForAnimation(objectID);
    
    cocos2d::CCArray* mainArray = (cocos2d::CCArray*)m_mainFramesForAnimation->objectForKey(key);
    cocos2d::CCArray* detailArray = (cocos2d::CCArray*)m_detailFramesForAnimation->objectForKey(key);
    
    if (mainArray && mainArray->count() > 0)
    {
        cocos2d::CCString* lastMain = (cocos2d::CCString*)mainArray->lastObject();
        cocos2d::CCString* lastDetail = detailArray ? (cocos2d::CCString*)detailArray->lastObject() : nullptr;
        
        addCustomAnimationFrame(objectID, frames + 1, lastMain->getCString(), lastDetail ? lastDetail->getCString() : "");
    }
}

void GameManager::reloadAll(bool switchingModes, bool toFullscreen, bool unused)
{
    m_reloading = (void*)unused;
    m_fix = (void*)false;
    m_toFullscreen = (void*)toFullscreen;
    m_switchModes = (void*)switchingModes;
    m_reloadTextures = (void*)true;
    
    cocos2d::CCDirector::sharedDirector()->popToRootScene();
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    cocos2d::CCDirector::sharedDirector()->replaceScene(scene);
    
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.0f);
    cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(GameManager::reloadAllStep2));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
    m_pActionManager->addAction(seq, this, false);
}

void GameManager::reloadAllStep2()
{
    if (m_reloadTextures)
    {
        cocos2d::CCDirector::sharedDirector()->removeStatsLabel();
        CCContentManager::sharedManager()->clearCache();
        cocos2d::CCTextureCache::purgeSharedTextureCache();
        cocos2d::CCSpriteFrameCache::purgeSharedSpriteFrameCache();
        cocos2d::CCLabelBMFont::purgeCachedData();
        BitmapFontCache::purgeSharedFontCache();
        CCAnimateFrameCache::sharedSpriteFrameCache()->removeSpriteFrames();
        cocos2d::CCFileUtils::purgeFileUtils();
        cocos2d::CCFileUtils::sharedFileUtils();
        ObjectManager::purgeObjectManager();
        
        m_loadedGroundID = 0;
        m_loadedBgID = 0;
        m_loadedMG = 0;
        m_loadedDeathEffect = 0;
        m_iconLoadCounts.clear();
    }
    
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.0f);
    cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(GameManager::reloadAllStep3));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
    m_pActionManager->addAction(seq, this, false);
}

void GameManager::reloadAllStep3()
{
    if (m_switchModes)
    {
        PlatformToolbox::toggleFullScreen((bool)m_toFullscreen, (bool)m_reloading, (bool)m_fix);
    }
    
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.0f);
    cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(GameManager::reloadAllStep4));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
    m_pActionManager->addAction(seq, this, false);
}

void GameManager::reloadAllStep4()
{
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.0f);
    cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(GameManager::reloadAllStep5));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
    m_pActionManager->addAction(seq, this, false);
}

void GameManager::reloadAllStep5()
{
    cocos2d::CCScene* scene;
    if (m_reloadTextures)
    {
        scene = LoadingLayer::scene(true);
    }
    else
    {
        scene = MenuLayer::scene(true);
    }
    cocos2d::CCDirector::sharedDirector()->replaceScene(scene);
}

void GameManager::reloadMenu()
{
    cocos2d::CCScene* scene = MenuLayer::scene(true);
    cocos2d::CCDirector::sharedDirector()->replaceScene(scene);
}

void GameManager::queueReloadMenu()
{
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.1f);
    cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(GameManager::reloadMenu));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
    m_pActionManager->addAction(seq, this, false);
}

void GameManager::returnToLastScene(GJGameLevel* level)
{
    cocos2d::CCScene* scene = nullptr;
    cocos2d::CCTransitionFade* transition = nullptr;
    
    switch (m_sceneEnum)
    {
        case 1:
            scene = CreatorLayer::scene();
            transition = cocos2d::CCTransitionFade::create(0.5f, scene);
            cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
            break;
        case 2:
        case 14:
            {
                GJSearchObject* searchObj = GJSearchObject::create((GJSearchObject*)(&qword_60 + (m_sceneEnum == 14 ? 6 : 2)), SearchType::Search);
                scene = LevelBrowserLayer::scene(searchObj);
                transition = cocos2d::CCTransitionFade::create(0.5f, scene);
                cocos2d::CCDirector::sharedDirector()->pushScene(transition);
            }
            break;
        case 3:
            if (!level)
            {
                if (m_searchObjectBool)
                {
                    m_searchObjectBool = false;
                    GameLevelManager* manager = GameLevelManager::sharedState();
                    scene = manager->getSearchScene(manager->m_searchKey.c_str());
                }
                if (!scene)
                {
                    scene = CreatorLayer::scene();
                }
                transition = cocos2d::CCTransitionFade::create(0.5f, scene);
                cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
            }
            else
            {
                if (level->m_levelType == Editor)
                {
                    scene = EditLevelLayer::scene(level);
                }
                else
                {
                    scene = LevelInfoLayer::scene(level, false);
                }
                transition = cocos2d::CCTransitionFade::create(0.5f, scene);
                cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
            }
            fadeInMenuMusic();
            break;
        case 7:
            {
                GameLevelManager* manager = GameLevelManager::sharedState();
                scene = manager->getSearchScene(manager->m_searchKey.c_str());
                if (!scene)
                {
                    scene = MenuLayer::scene(false);
                }
                transition = cocos2d::CCTransitionFade::create(0.5f, scene);
                cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
                fadeInMenuMusic();
            }
            break;
        case 9:
            {
                int levelID = 0;
                if (level)
                {
                    int id = level->m_levelID.random - level->m_levelID.seed;
                    if (id > 1) levelID = id - 1;
                }
                scene = LevelSelectLayer::scene(levelID);
                transition = cocos2d::CCTransitionFade::create(0.5f, scene);
                cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
                fadeInMenuMusic();
            }
            break;
        case 12:
            scene = SecretLayer2::scene();
            transition = cocos2d::CCTransitionFade::create(0.5f, scene);
            cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
            break;
        case 15:
            scene = LevelAreaInnerLayer::scene(true);
            transition = cocos2d::CCTransitionFade::create(0.5f, scene);
            cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
            break;
        default:
            scene = MenuLayer::scene(false);
            transition = cocos2d::CCTransitionFade::create(0.5f, scene);
            cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
            fadeInMenuMusic();
            break;
    }
}

bool GameManager::safePopScene()
{
    if (cocos2d::CCDirector::sharedDirector()->sceneCount() <= 1)
    {
        cocos2d::CCScene* scene = MenuLayer::scene(false);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        return cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    }
    else
    {
        return cocos2d::CCDirector::sharedDirector()->popSceneWithTransition(0.5f, kPopTransitionFade);
    }
}

void GameManager::switchScreenMode(bool fullscreen, bool borderless, bool fix, bool unused)
{
    reloadAll(true, fullscreen, borderless);
}

void GameManager::playMenuMusic()
{
    if (getGameVariable("0122"))
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
        return;
    }
    
    std::string musicFile = getMenuMusicFile();
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    
    if (!engine->isMusicPlaying(musicFile))
    {
        engine->playMusic(musicFile, true, 1.0f, 0.0f);
    }
}

void GameManager::fadeInMenuMusic()
{
    if (getGameVariable("0122"))
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    }
    else
    {
        std::string musicFile = getMenuMusicFile();
        fadeInMusic(musicFile);
    }
}

void GameManager::fadeInMusic(std::string path)
{
    if (path.empty()) return;
    
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    if (!engine->isMusicPlaying(path))
    {
        engine->playMusic(path, true, 1.0f, 0.0f);
    }
}

std::string GameManager::getMenuMusicFile()
{
    if (m_customMenuSongID > 0)
    {
        MusicDownloadManager* manager = MusicDownloadManager::sharedState();
        if (manager->isSongDownloaded(m_customMenuSongID))
        {
            return manager->pathForSong(m_customMenuSongID);
        }
        m_customMenuSongID = 0;
    }
    
    return "menuLoop.mp3";
}

std::string GameManager::getPracticeMusicFile()
{
    if (m_customPracticeSongID > 0)
    {
        MusicDownloadManager* manager = MusicDownloadManager::sharedState();
        if (manager->isSongDownloaded(m_customPracticeSongID))
        {
            return manager->pathForSong(m_customPracticeSongID);
        }
        m_customPracticeSongID = 0;
    }
    
    return "StayInsideMe.mp3";
}

void GameManager::updateMusic()
{
    FMODAudioEngine::sharedEngine()->setBackgroundMusicVolume(m_bgVolume);
    FMODAudioEngine::sharedEngine()->setEffectsVolume(m_sfxVolume);
    FMODAudioEngine::sharedEngine()->m_musicOffset = (int)m_timeOffset;
}

void GameManager::resumeAudio()
{
    FMODAudioEngine::sharedEngine()->resumeAudio();
    FMODAudioEngine::sharedEngine()->resumeAllAudio();
    AppDelegate::get()->resumeSound();
    FMODAudioEngine::sharedEngine()->m_system->update();
}

void GameManager::resumeAudioDelayed()
{
    stopActionByTag(11);
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.05f);
    cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(GameManager::resumeAudio));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
    seq->setTag(11);
    m_pActionManager->addAction(seq, this, false);
}

int GameManager::getNextUniqueObjectKey()
{
    cocos2d::CCArray* keys = getOrderedCustomObjectKeys();
    if (!keys || keys->count() == 0)
    {
        return -1;
    }
    
    int minKey = 0;
    for (unsigned int i = 0; i < keys->count(); i++)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)keys->objectAtIndex(i);
        int key = str->intValue();
        if (i == 0 || key < minKey)
        {
            minKey = key;
        }
    }
    
    return minKey - 1;
}

void GameManager::addNewCustomObject(std::string str)
{
    int key = getNextUniqueObjectKey();
    cocos2d::CCString* keyStr = cocos2d::CCString::createWithFormat("%i", key);
    m_customObjectDict->setObject(cocos2d::CCString::create(str), keyStr->getCString());
}

void GameManager::removeCustomObject(int key)
{
    cocos2d::CCString* keyStr = cocos2d::CCString::createWithFormat("%i", key);
    m_customObjectDict->removeObjectForKey(keyStr->getCString());
}

std::string GameManager::stringForCustomObject(int customObjectID)
{
    cocos2d::CCString* keyStr = cocos2d::CCString::createWithFormat("%i", customObjectID);
    cocos2d::CCString* value = (cocos2d::CCString*)m_customObjectDict->objectForKey(keyStr->getCString());
    return value ? value->getCString() : "";
}

void GameManager::switchCustomObjects(int key1, int key2)
{
    cocos2d::CCString* keyStr1 = cocos2d::CCString::createWithFormat("%i", key1);
    cocos2d::CCString* keyStr2 = cocos2d::CCString::createWithFormat("%i", key2);
    
    cocos2d::CCObject* obj1 = m_customObjectDict->objectForKey(keyStr1->getCString());
    cocos2d::CCObject* obj2 = m_customObjectDict->objectForKey(keyStr2->getCString());
    
    if (obj1) obj1->retain();
    
    m_customObjectDict->setObject(obj2, keyStr1->getCString());
    m_customObjectDict->setObject(obj1, keyStr2->getCString());
    
    if (obj1) obj1->release();
}

int GameManager::reorderKey(int index, bool up)
{
    int nextKey = getNextUsedKey(index, up);
    if (nextKey != 0)
    {
        switchCustomObjects(index, nextKey);
        return nextKey;
    }
    return index;
}

int GameManager::getNextUsedKey(int index, bool up)
{
    cocos2d::CCArray* keys = getOrderedCustomObjectKeys();
    if (!keys) return 0;
    
    for (unsigned int i = 0; i < keys->count(); i++)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)keys->objectAtIndex(i);
        int key = str->intValue();
        if (key == index)
        {
            if (up && i > 0)
            {
                cocos2d::CCString* prev = (cocos2d::CCString*)keys->objectAtIndex(i - 1);
                return prev->intValue();
            }
            else if (!up && i + 1 < keys->count())
            {
                cocos2d::CCString* next = (cocos2d::CCString*)keys->objectAtIndex(i + 1);
                return next->intValue();
            }
            break;
        }
    }
    
    return 0;
}

cocos2d::CCArray* GameManager::getOrderedCustomObjectKeys()
{
    cocos2d::CCArray* keys = m_customObjectDict->allKeys();
    if (keys->count() > 0)
    {
        // Sort keys
        keys->sort(customObjectCompare);
    }
    return keys;
}

int customObjectCompare(const void* a, const void* b)
{
    cocos2d::CCString* strA = *(cocos2d::CCString**)a;
    cocos2d::CCString* strB = *(cocos2d::CCString**)b;
    return strA->intValue() - strB->intValue();
}

void GameManager::followTwitter()
{
    if (GameToolbox::doWeHaveInternet())
    {
        cocos2d::CCApplication::sharedApplication()->openURL("https://twitter.com/robtopgames");
        m_clickedTwitter = true;
        struct timespec ts;
        clock_gettime(0, &ts);
        m_socialsDuration = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
}

void GameManager::followTwitch()
{
    if (GameToolbox::doWeHaveInternet())
    {
        cocos2d::CCApplication::sharedApplication()->openURL("https://twitch.tv/robtopgames");
        m_clickedTwitch = true;
        struct timespec ts;
        clock_gettime(0, &ts);
        m_socialsDuration = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
}

void GameManager::joinDiscord()
{
    if (GameToolbox::doWeHaveInternet())
    {
        cocos2d::CCApplication::sharedApplication()->openURL("https://discord.gg/geometrydash");
        m_clickedDiscord = true;
        struct timespec ts;
        clock_gettime(0, &ts);
        m_socialsDuration = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
}

void GameManager::joinReddit()
{
    if (GameToolbox::doWeHaveInternet())
    {
        cocos2d::CCApplication::sharedApplication()->openURL("https://reddit.com/r/geometrydash");
        m_clickedReddit = true;
        struct timespec ts;
        clock_gettime(0, &ts);
        m_socialsDuration = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
}

void GameManager::likeFacebook()
{
    if (GameToolbox::doWeHaveInternet())
    {
        cocos2d::CCApplication::sharedApplication()->openURL("https://facebook.com/geometrydash");
        m_clickedFacebook = true;
        struct timespec ts;
        clock_gettime(0, &ts);
        m_socialsDuration = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
}

void GameManager::subYouTube()
{
    if (GameToolbox::doWeHaveInternet())
    {
        cocos2d::CCApplication::sharedApplication()->openURL("https://youtube.com/robtopgames");
        m_clickedYouTube = true;
        struct timespec ts;
        clock_gettime(0, &ts);
        m_socialsDuration = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
}

void GameManager::openEditorGuide()
{
    m_showedEditorGuide = true;
    cocos2d::CCApplication::sharedApplication()->openURL("https://geometrydash.com/editor-guide");
}

void GameManager::rateGame()
{
    if (GameToolbox::doWeHaveInternet())
    {
        GameToolbox::openAppPage();
        m_ratedGame = true;
        m_hasRatedGame = true;
    }
}

int GameManager::generateSecretNumber()
{
    int num = 0;
    for (int i = 0; i < 7; i++)
    {
        int digit = (int)((float)rand() * 4.6566e-10f * 8.0f) + 1;
        num += (int)(digit * powf(10.0f, i));
    }
    
    int seed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    m_secretNumber.random = seed;
    m_secretNumber.seed = num + seed;
    
    return seed;
}

void GameManager::unlockedPremium()
{
}

void GameManager::eventUnlockFeature(const char* key)
{
}

void GameManager::itemPurchased(const char* key)
{
}

void GameManager::claimItemsResponse(std::string str)
{
}

void GameManager::checkSteamAchievementUnlock()
{
}

void GameManager::verifyAchievementUnlocks()
{
    AchievementManager* manager = AchievementManager::sharedState();
    cocos2d::CCArray* achievements = manager->getAllAchievements();
    
    for (unsigned int i = 0; i < achievements->count(); i++)
    {
        cocos2d::CCDictionary* dict = (cocos2d::CCDictionary*)achievements->objectAtIndex(i);
        std::string identifier = dict->valueForKey("identifier")->getCString();
        
        if (manager->isAchievementEarned(identifier.c_str()))
        {
            completedAchievement(identifier);
        }
    }
}

void GameManager::verifyCoinUnlocks()
{
    int coins = 0;
    
    for (int i = 1; i < 23; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, false);
        if (GameStatsManager::sharedState()->hasCompletedLevel(level))
        {
            for (int c = 1; c <= 3; c++)
            {
                const char* key = GJGameLevel::getCoinKey(level, c);
                if (GameStatsManager::sharedState()->hasSecretCoin(key))
                {
                    coins++;
                }
            }
        }
    }
    
    for (int i = 5001; i < 5005; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, false);
        if (GameStatsManager::sharedState()->hasCompletedLevel(level))
        {
            for (int c = 1; c <= 3; c++)
            {
                const char* key = GJGameLevel::getCoinKey(level, c);
                if (GameStatsManager::sharedState()->hasSecretCoin(key))
                {
                    coins++;
                }
            }
        }
    }
    
    if (GameStatsManager::sharedState()->hasSecretCoin("secret04")) coins++;
    if (GameStatsManager::sharedState()->hasSecretCoin("secret06")) coins++;
    if (GameStatsManager::sharedState()->hasSecretCoin("secretB03")) coins++;
    
    int mapPackCoins = 0;
    // Calcular monedas de map packs
    
    GameStatsManager::sharedState()->setStat("8", coins + mapPackCoins);
}

void GameManager::verifyStarUnlocks()
{
    for (int i = 1; i < 23; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, false);
        if (GameStatsManager::sharedState()->hasCompletedLevel(level))
        {
            GameStatsManager::sharedState()->completedStarLevel(level);
        }
    }
}

void GameManager::verifySyncedCoins()
{
    int coins = 0;
    
    for (int i = 1; i < 23; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, false);
        if (GameStatsManager::sharedState()->hasCompletedLevel(level))
        {
            for (int c = 1; c <= 3; c++)
            {
                const char* key = GJGameLevel::getCoinKey(level, c);
                if (GameStatsManager::sharedState()->hasSecretCoin(key))
                {
                    coins++;
                }
            }
        }
    }
    
    for (int i = 5001; i < 5005; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, false);
        if (GameStatsManager::sharedState()->hasCompletedLevel(level))
        {
            for (int c = 1; c <= 3; c++)
            {
                const char* key = GJGameLevel::getCoinKey(level, c);
                if (GameStatsManager::sharedState()->hasSecretCoin(key))
                {
                    coins++;
                }
            }
        }
    }
    
    if (GameStatsManager::sharedState()->hasSecretCoin("secret04")) coins++;
    if (GameStatsManager::sharedState()->hasSecretCoin("secret06")) coins++;
    if (GameStatsManager::sharedState()->hasSecretCoin("secretB03")) coins++;
    
    GameStatsManager::sharedState()->setStatIfHigher("8", coins);
}

void GameManager::recountUserStats(std::string str)
{
    bool dontNotify = AchievementManager::sharedState()->m_dontNotify;
    AchievementManager::sharedState()->m_dontNotify = true;
    
    bool skipIncrement = GameStatsManager::sharedState()->m_skipIncrementChallenge;
    GameStatsManager::sharedState()->m_skipIncrementChallenge = true;
    
    // Recount stars, demons, etc.
    int starCount = 0;
    int demonCount = 0;
    int insaneCount = 0;
    int dailyCount = 0;
    
    // Main levels
    for (int i = 1; i < 23; i++)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getMainLevel(i, true);
        if (GameStatsManager::sharedState()->hasCompletedLevel(level))
        {
            int stars = level->m_stars.random - level->m_stars.seed;
            if (stars < 0) stars = 0;
            if (stars > 20) stars = 20;
            
            if (level->isPlatformer())
            {
                starCount += stars;
            }
            else
            {
                starCount += stars;
            }
            
            if (level->m_demon.random - level->m_demon.seed == 1)
            {
                demonCount++;
            }
            if (stars >= 8 && stars <= 9)
            {
                insaneCount++;
            }
        }
    }
    
    // Online levels
    GameLevelManager* levelManager = GameLevelManager::sharedState();
    cocos2d::CCDictionary* onlineLevels = levelManager->m_onlineLevels;
    if (onlineLevels)
    {
        for (cocos2d::CCDictElement* elem = (cocos2d::CCDictElement*)onlineLevels->m_pElements; elem; elem = (cocos2d::CCDictElement*)elem->hh.next)
        {
            GJGameLevel* level = (GJGameLevel*)elem->m_pObject;
            if (level && GameStatsManager::sharedState()->hasCompletedLevel(level))
            {
                int stars = level->m_stars.random - level->m_stars.seed;
                if (stars < 0) stars = 0;
                if (stars > 10) stars = 10;
                
                if (level->isPlatformer())
                {
                    starCount += stars;
                }
                else
                {
                    starCount += stars;
                }
                
                if (level->m_demon.random - level->m_demon.seed == 1)
                {
                    demonCount++;
                }
                if (stars >= 8 && stars <= 9)
                {
                    insaneCount++;
                }
                if (level->m_dailyID.random - level->m_dailyID.seed >= 1 && level->m_dailyID.random - level->m_dailyID.seed <= 100000)
                {
                    dailyCount++;
                }
            }
        }
    }
    
    // Map packs
    cocos2d::CCDictionary* starPackDict = LevelTools::createStarPackDict();
    cocos2d::CCArray* packKeys = starPackDict->allKeys();
    for (unsigned int i = 0; i < packKeys->count(); i++)
    {
        int packID = (int)packKeys->objectAtIndex(i)->getTag();
        int stars = cocos2d::CCString::intValue((cocos2d::CCString*)starPackDict->valueForKey(std::to_string(packID)));
        if (stars > 10) stars = 10;
        if (stars < 0) stars = 0;
        GameStatsManager::sharedState()->setStarsForMapPack(packID, stars);
    }
    
    cocos2d::CCArray* completedPacks = GameStatsManager::sharedState()->getCompletedMapPacks();
    for (unsigned int i = 0; i < completedPacks->count(); i++)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)completedPacks->objectAtIndex(i);
        int packID = str->intValue();
        starCount += GameStatsManager::sharedState()->starsForMapPack(packID);
    }
    
    GameStatsManager::sharedState()->setStat("6", starCount);
    GameStatsManager::sharedState()->setStat("28", 0); // platformer stars
    GameStatsManager::sharedState()->setStat("5", demonCount);
    GameStatsManager::sharedState()->setStat("42", insaneCount);
    GameStatsManager::sharedState()->setStat("15", dailyCount);
    
    checkUsedIcons();
    
    GameStatsManager::sharedState()->m_skipIncrementChallenge = skipIncrement;
    AchievementManager::sharedState()->m_dontNotify = dontNotify;
}

void GameManager::syncPlatformAchievements()
{
    if (m_googlePlaySignedIn) return;
    if (!PlatformToolbox::isLocalPlayerAuthenticated() && !PlatformToolbox::isSignedInGooglePlay()) return;
    
    m_googlePlaySignedIn = true;
    
    AchievementManager* manager = AchievementManager::sharedState();
    cocos2d::CCArray* achievements = manager->getAllAchievements();
    
    for (unsigned int i = 0; i < achievements->count(); i++)
    {
        cocos2d::CCDictionary* dict = (cocos2d::CCDictionary*)achievements->objectAtIndex(i);
        std::string identifier = dict->valueForKey("identifier")->getCString();
        int percent = manager->percentForAchievement(identifier.c_str());
        manager->reportPlatformAchievementWithID(identifier.c_str(), percent);
    }
}

void GameManager::completedAchievement(std::string key)
{
    int id = 0;
    UnlockType type = UnlockType();
    
    getUnlockForAchievement(key, &id, &type);
    
    if (type)
    {
        if (type == Col1 || type == Col2)
        {
            unlockColor(id, type);
        }
        else
        {
            IconType iconType = unlockTypeToIconType(type);
            unlockIcon(id, iconType);
        }
    }
}

void GameManager::getUnlockForAchievement(std::string key, int* id, UnlockType* type)
{
    *id = 0;
    *type = UnlockType();
    
    AchievementManager* manager = AchievementManager::sharedState();
    cocos2d::CCDictionary* achievement = manager->getAchievementsWithID(key.c_str());
    if (!achievement) return;
    
    cocos2d::CCString* iconStr = (cocos2d::CCString*)achievement->valueForKey("icon");
    if (!iconStr) return;
    
    std::string icon = iconStr->getCString();
    std::vector<std::string> parts;
    GameToolbox::split(icon, "_", parts);
    
    if (parts.size() > 1)
    {
        *id = atoi(parts[1].c_str());
        
        if (parts[0] == "icon")
        {
            *type = CubeType;
        }
        else if (parts[0] == "ship")
        {
            *type = ShipType;
        }
        else if (parts[0] == "ball")
        {
            *type = BallType;
        }
        else if (parts[0] == "bird")
        {
            *type = UfoType;
        }
        else if (parts[0] == "dart")
        {
            *type = WaveType;
        }
        else if (parts[0] == "robot")
        {
            *type = RobotType;
        }
        else if (parts[0] == "spider")
        {
            *type = SpiderType;
        }
        else if (parts[0] == "swing")
        {
            *type = SwingType;
        }
        else if (parts[0] == "jetpack")
        {
            *type = JetpackType;
        }
        else if (parts[0] == "death")
        {
            *type = DeathType;
        }
        else if (parts[0] == "special")
        {
            *type = StreakType;
        }
        else if (parts[0] == "shipstreak")
        {
            *type = ShipFireType;
        }
        else if (parts[0] == "color")
        {
            *type = Col1;
        }
        else if (parts[0] == "color2")
        {
            *type = Col2;
        }
    }
}

void GameManager::reportAchievementWithID(const char* key, int percent, bool dontNotify)
{
    AchievementManager* manager = AchievementManager::sharedState();
    if (manager->isAchievementEarned(key)) return;
    
    if (percent > 99)
    {
        completedAchievement(key);
    }
    
    manager->reportAchievementWithID(key, percent, dontNotify);
}

void GameManager::resetAchievement(std::string key)
{
    int id = 0;
    UnlockType type = UnlockType();
    
    getUnlockForAchievement(key, &id, &type);
    
    if (type)
    {
        if (type == Col1 || type == Col2)
        {
            lockColor(id, type);
        }
        else
        {
            IconType iconType = unlockTypeToIconType(type);
            lockIcon(id, iconType);
        }
    }
    
    AchievementManager::sharedState()->resetAchievement(key.c_str());
}

void GameManager::reportPercentageForLevel(int levelID, int percentage, bool isPlatformer)
{
    const char* achievement = nullptr;
    
    if (percentage > 94 && !isPlatformer)
    {
        // Normal level achievements
        switch (levelID)
        {
            case 1: achievement = "geometry.ach.level01a"; break;
            case 2: achievement = "geometry.ach.level02a"; break;
            case 3: achievement = "geometry.ach.level03a"; break;
            case 4: achievement = "geometry.ach.level04a"; break;
            case 5: achievement = "geometry.ach.level05a"; break;
            case 6: achievement = "geometry.ach.level06a"; break;
            case 7: achievement = "geometry.ach.level07a"; break;
            case 8: achievement = "geometry.ach.level08a"; break;
            case 9: achievement = "geometry.ach.level09a"; break;
            case 10: achievement = "geometry.ach.level10a"; break;
            case 11: achievement = "geometry.ach.level11a"; break;
            case 12: achievement = "geometry.ach.level12a"; break;
            case 13: achievement = "geometry.ach.level13a"; break;
            case 14: achievement = "geometry.ach.level14a"; break;
            case 15: achievement = "geometry.ach.level15a"; break;
            case 16: achievement = "geometry.ach.level16a"; break;
            case 17: achievement = "geometry.ach.level17a"; break;
            case 18: achievement = "geometry.ach.level18a"; break;
            case 19: achievement = "geometry.ach.level19a"; break;
            case 20: achievement = "geometry.ach.level20a"; break;
            case 21: achievement = "geometry.ach.level21a"; break;
            case 22: achievement = "geometry.ach.level22a"; break;
        }
    }
    else if (percentage > 99)
    {
        // Special achievement
        reportAchievementWithID("geometry.ach.special01", 100, false);
        
        // Level specific achievements
        switch (levelID)
        {
            case 1: achievement = "geometry.ach.level01b"; break;
            case 2: achievement = "geometry.ach.level02b"; break;
            case 3: achievement = "geometry.ach.level03b"; break;
            case 4: achievement = "geometry.ach.level04b"; break;
            case 5: achievement = "geometry.ach.level05b"; break;
            case 6: achievement = "geometry.ach.level06b"; break;
            case 7: achievement = "geometry.ach.level07b"; break;
            case 8: achievement = "geometry.ach.level08b"; break;
            case 9: achievement = "geometry.ach.level09b"; break;
            case 10: achievement = "geometry.ach.level10b"; break;
            case 11: achievement = "geometry.ach.level11b"; break;
            case 12: achievement = "geometry.ach.level12b"; break;
            case 13: achievement = "geometry.ach.level13b"; break;
            case 14: achievement = "geometry.ach.level14b"; break;
            case 15: achievement = "geometry.ach.level15b"; break;
            case 16: achievement = "geometry.ach.level16b"; break;
            case 17: achievement = "geometry.ach.level17b"; break;
            case 18: achievement = "geometry.ach.level18b"; break;
            case 19: achievement = "geometry.ach.level19b"; break;
            case 20: achievement = "geometry.ach.level20b"; break;
            case 21: achievement = "geometry.ach.level21b"; break;
            case 22: achievement = "geometry.ach.level22b"; break;
        }
    }
    
    if (achievement)
    {
        reportAchievementWithID(achievement, percentage, false);
    }
}

bool GameManager::levelIsPremium(int unk1, int unk2)
{
    return false;
}

cocos2d::CCSize GameManager::resolutionForKey(int key)
{
    switch (key)
    {
        case 1: return cocos2d::CCSize(640, 480);
        case 2: return cocos2d::CCSize(720, 480);
        case 3: return cocos2d::CCSize(720, 576);
        case 4: return cocos2d::CCSize(800, 600);
        case 5: return cocos2d::CCSize(1024, 768);
        case 6: return cocos2d::CCSize(1152, 864);
        case 7: return cocos2d::CCSize(1176, 664);
        case 8: return cocos2d::CCSize(1280, 720);
        case 9: return cocos2d::CCSize(1280, 768);
        case 10: return cocos2d::CCSize(1280, 800);
        case 11: return cocos2d::CCSize(1280, 960);
        case 12: return cocos2d::CCSize(1280, 1024);
        case 13: return cocos2d::CCSize(1360, 768);
        case 14: return cocos2d::CCSize(1366, 768);
        case 15: return cocos2d::CCSize(1440, 900);
        case 16: return cocos2d::CCSize(1600, 900);
        case 17: return cocos2d::CCSize(1600, 1024);
        case 18: return cocos2d::CCSize(1600, 1200);
        case 19: return cocos2d::CCSize(1680, 1050);
        case 20: return cocos2d::CCSize(1768, 992);
        case 21: return cocos2d::CCSize(1920, 1080);
        case 22: return cocos2d::CCSize(1920, 1200);
        case 23: return cocos2d::CCSize(1920, 1440);
        case 24: return cocos2d::CCSize(2048, 1536);
        case 25: return cocos2d::CCSize(2560, 1440);
        case 26: return cocos2d::CCSize(2560, 1600);
        case 27: return cocos2d::CCSize(3840, 2160);
        default: return PlatformToolbox::getDisplaySize();
    }
}

cocos2d::ccColor3B GameManager::colorForIdx(int index) {
    switch (index) {
        case 0: return cocos2d::ccc3(125, 255, 0);
        case 1: return cocos2d::ccc3(0, 255, 0);
        case 2: return cocos2d::ccc3(0, 255, 125);
        case 3: return cocos2d::ccc3(0, 255, 255);
        case 4: return cocos2d::ccc3(0, 125, 255);
        case 5: return cocos2d::ccc3(0, 0, 255);
        case 6: return cocos2d::ccc3(125, 0, 255);
        case 7: return cocos2d::ccc3(255, 0, 255);
        case 8: return cocos2d::ccc3(255, 0, 125);
        case 9: return cocos2d::ccc3(255, 0, 0);
        case 10: return cocos2d::ccc3(255, 125, 0);
        case 11: return cocos2d::ccc3(255, 255, 0);
        case 12: return cocos2d::ccc3(255, 255, 255);
        case 13: return cocos2d::ccc3(185, 0, 255);
        case 14: return cocos2d::ccc3(255, 185, 0);
        case 15: return cocos2d::ccc3(0, 0, 0);
        case 16: return cocos2d::ccc3(0, 200, 255);
        case 17: return cocos2d::ccc3(175, 175, 175);
        case 18: return cocos2d::ccc3(90, 90, 90);
        case 19: return cocos2d::ccc3(255, 125, 125);
        case 20: return cocos2d::ccc3(0, 175, 75);
        case 21: return cocos2d::ccc3(0, 125, 125);
        case 22: return cocos2d::ccc3(0, 75, 175);
        case 23: return cocos2d::ccc3(75, 0, 175);
        case 24: return cocos2d::ccc3(125, 0, 125);
        case 25: return cocos2d::ccc3(175, 0, 75);
        case 26: return cocos2d::ccc3(175, 75, 0);
        case 27: return cocos2d::ccc3(125, 125, 0);
        case 28: return cocos2d::ccc3(75, 175, 0);
        case 29: return cocos2d::ccc3(255, 75, 0);
        case 30: return cocos2d::ccc3(150, 50, 0);
        case 31: return cocos2d::ccc3(150, 100, 0);
        case 32: return cocos2d::ccc3(100, 150, 0);
        case 33: return cocos2d::ccc3(0, 150, 100);
        case 34: return cocos2d::ccc3(0, 100, 150);
        case 35: return cocos2d::ccc3(100, 0, 150);
        case 36: return cocos2d::ccc3(150, 0, 100);
        case 37: return cocos2d::ccc3(150, 0, 0);
        case 38: return cocos2d::ccc3(0, 150, 0);
        case 39: return cocos2d::ccc3(0, 0, 150);
        case 40: return cocos2d::ccc3(125, 255, 175);
        case 41: return cocos2d::ccc3(125, 125, 255);
        case 42: return cocos2d::ccc3(255, 250, 127);
        case 43: return cocos2d::ccc3(250, 127, 255);
        case 44: return cocos2d::ccc3(0, 255, 192);
        case 45: return cocos2d::ccc3(80, 50, 14);
        case 46: return cocos2d::ccc3(205, 165, 118);
        case 47: return cocos2d::ccc3(182, 128, 255);
        case 48: return cocos2d::ccc3(255, 58, 58);
        case 49: return cocos2d::ccc3(77, 77, 143);
        case 50: return cocos2d::ccc3(0, 10, 76);
        case 51: return cocos2d::ccc3(253, 212, 206);
        case 52: return cocos2d::ccc3(190, 181, 255);
        case 53: return cocos2d::ccc3(112, 0, 0);
        case 54: return cocos2d::ccc3(82, 2, 0);
        case 55: return cocos2d::ccc3(56, 1, 6);
        case 56: return cocos2d::ccc3(128, 79, 79);
        case 57: return cocos2d::ccc3(122, 53, 53);
        case 58: return cocos2d::ccc3(81, 36, 36);
        case 59: return cocos2d::ccc3(163, 98, 70);
        case 60: return cocos2d::ccc3(117, 73, 54);
        case 61: return cocos2d::ccc3(86, 53, 40);
        case 62: return cocos2d::ccc3(255, 185, 114);
        case 63: return cocos2d::ccc3(255, 160, 64);
        case 64: return cocos2d::ccc3(102, 49, 30);
        case 65: return cocos2d::ccc3(91, 39, 0);
        case 66: return cocos2d::ccc3(71, 32, 0);
        case 67: return cocos2d::ccc3(167, 123, 77);
        case 68: return cocos2d::ccc3(109, 83, 57);
        case 69: return cocos2d::ccc3(81, 62, 42);
        case 70: return cocos2d::ccc3(255, 255, 192);
        case 71: return cocos2d::ccc3(253, 224, 160);
        case 72: return cocos2d::ccc3(192, 255, 160);
        case 73: return cocos2d::ccc3(177, 255, 109);
        case 74: return cocos2d::ccc3(192, 255, 224);
        case 75: return cocos2d::ccc3(148, 255, 228);
        case 76: return cocos2d::ccc3(67, 161, 138);
        case 77: return cocos2d::ccc3(49, 109, 95);
        case 78: return cocos2d::ccc3(38, 84, 73);
        case 79: return cocos2d::ccc3(0, 96, 0);
        case 80: return cocos2d::ccc3(0, 64, 0);
        case 81: return cocos2d::ccc3(0, 96, 96);
        case 82: return cocos2d::ccc3(0, 64, 64);
        case 83: return cocos2d::ccc3(160, 255, 255);
        case 84: return cocos2d::ccc3(1, 7, 112);
        case 85: return cocos2d::ccc3(0, 73, 109);
        case 86: return cocos2d::ccc3(0, 50, 76);
        case 87: return cocos2d::ccc3(0, 38, 56);
        case 88: return cocos2d::ccc3(80, 128, 173);
        case 89: return cocos2d::ccc3(51, 83, 117);
        case 90: return cocos2d::ccc3(35, 60, 86);
        case 91: return cocos2d::ccc3(224, 224, 224);
        case 92: return cocos2d::ccc3(61, 6, 140);
        case 93: return cocos2d::ccc3(55, 8, 96);
        case 94: return cocos2d::ccc3(64, 64, 64);
        case 95: return cocos2d::ccc3(111, 73, 164);
        case 96: return cocos2d::ccc3(84, 54, 127);
        case 97: return cocos2d::ccc3(66, 42, 99);
        case 98: return cocos2d::ccc3(252, 181, 255);
        case 99: return cocos2d::ccc3(175, 87, 175);
        case 100: return cocos2d::ccc3(130, 67, 130);
        case 101: return cocos2d::ccc3(94, 49, 94);
        case 102: return cocos2d::ccc3(128, 128, 128);
        case 103: return cocos2d::ccc3(102, 3, 62);
        case 104: return cocos2d::ccc3(71, 1, 52);
        case 105: return cocos2d::ccc3(210, 255, 50);
        case 106: return cocos2d::ccc3(118, 189, 255);
        default: return cocos2d::ccc3(255, 255, 255);
    }
}

int GameManager::colorForPos(int pos)
{
    switch (pos)
    {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 16;
        case 5: return 4;
        case 6: return 5;
        case 7: return 6;
        case 8: return 13;
        case 9: return 7;
        case 10: return 8;
        case 11: return 9;
        case 12: return 29;
        case 13: return 10;
        case 14: return 14;
        case 15: return 11;
        case 16: return 12;
        case 17: return 17;
        case 18: return 18;
        case 19: return 15;
        case 20: return 27;
        case 21: return 32;
        case 22: return 28;
        case 23: return 38;
        case 24: return 20;
        case 25: return 33;
        case 26: return 21;
        case 27: return 34;
        case 28: return 22;
        case 29: return 39;
        case 30: return 23;
        case 31: return 35;
        case 32: return 24;
        case 33: return 36;
        case 34: return 25;
        case 35: return 37;
        case 36: return 30;
        case 37: return 26;
        case 38: return 31;
        case 39: return 19;
        default: return pos;
    }
}

void GameManager::addToGJLog(cocos2d::CCString* str)
{
}

void GameManager::clearGJLog()
{
    m_gjLog->removeAllObjects();
}

void GameManager::printGJLog()
{
}

bool GameManager::canShowRewardedVideo()
{
    if (!AdToolbox::hasCachedRewardedVideo()) return false;
    
    std::string reward = GameStatsManager::sharedState()->getNextVideoAdReward();
    return !reward.empty();
}

void GameManager::tryCacheAd()
{
    struct timespec ts;
    clock_gettime(0, &ts);
    double now = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    
    if (now - m_adCache >= 60.0)
    {
        clock_gettime(0, &ts);
        m_adCache = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
        AdToolbox::cacheInterstitial();
    }
}

void GameManager::saveAdTimer()
{
    struct timespec ts;
    clock_gettime(0, &ts);
    m_adTimer = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

void GameManager::resetAdTimer()
{
    m_adTimer = 0.0f;
}

void GameManager::shortenAdTimer(float time)
{
    m_adTimer -= time;
}

bool GameManager::shouldShowInterstitial(int unk1, int unk2, int unk3)
{
    return false;
}

bool GameManager::showInterstitial()
{
    return true;
}

bool GameManager::showInterstitialForced()
{
    return false;
}

bool GameManager::showMainMenuAd()
{
    return false;
}

void GameManager::tryShowInterstitial(int unk1, int unk2, int unk3)
{
    if (shouldShowInterstitial(unk1, unk2, unk3))
    {
        showInterstitial();
    }
}

void GameManager::rewardedVideoAdFinished(int unused)
{
    saveAdTimer();
    rewardedVideoHiddenDelayed();
}

void GameManager::rewardedVideoHidden()
{
    if (m_unk664 > 0)
    {
        GameStatsManager::sharedState()->unlockGauntlet(m_unk664);
        if (m_rewardedVideoDelegate)
        {
            m_rewardedVideoDelegate->rewardedVideoFinished();
        }
        m_unk664 = 0;
    }
    else
    {
        std::string reward = GameStatsManager::sharedState()->getNextVideoAdReward();
        
        if (!reward.empty())
        {
            GJRewardItem* item = GameStatsManager::sharedState()->unlockSpecialChest(reward);
            if (item)
            {
                RewardUnlockLayer* layer = RewardUnlockLayer::create(item->m_rewardType, false);
                cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
                int zOrder = 1000;
                if (cocos2d::CCScene::getHighestChildZ(scene) > 999)
                {
                    zOrder = cocos2d::CCScene::getHighestChildZ(scene) + 1;
                }
                scene->addChild(layer, zOrder);
                layer->showCollectReward(item);
                if (m_rewardedVideoDelegate)
                {
                    m_rewardedVideoDelegate->rewardedVideoFinished();
                }
            }
        }
        else
        {
            AchievementManager::sharedState()->m_dontNotify = true;
            GameStatsManager::sharedState()->collectVideoReward(100);
            AchievementManager::sharedState()->m_dontNotify = false;
            
            GJRewardItem* item = GJRewardItem::createSpecial(Small, 100, 0, (SpecialRewardItem)0, 0, (SpecialRewardItem)0, 0, 0, 0);
            RewardUnlockLayer* layer = RewardUnlockLayer::create(item->m_rewardType, false);
            
            if (m_rewardedVideoDelegate && m_rewardedVideoDelegate->shouldOffsetRewardCurrency())
            {
                layer->m_unkFloat = -60.0f;
            }
            
            cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
            int zOrder = 1000;
            if (cocos2d::CCScene::getHighestChildZ(scene) > 999)
            {
                zOrder = cocos2d::CCScene::getHighestChildZ(scene) + 1;
            }
            scene->addChild(layer, zOrder);
            layer->showCollectReward(item);
            
            if (m_rewardedVideoDelegate)
            {
                m_rewardedVideoDelegate->rewardedVideoFinished();
            }
        }
    }
}

void GameManager::rewardedVideoHiddenDelayed()
{
    stopActionByTag(10);
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.05f);
    cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(GameManager::rewardedVideoHidden));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
    seq->setTag(10);
    m_pActionManager->addAction(seq, this, false);
}

void GameManager::videoAdShowed()
{
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    if (engine->isMusicPlaying(0))
    {
        engine->pauseAllMusic(true);
        m_musicPaused = true;
    }
    else
    {
        m_musicPaused = false;
    }
}

void GameManager::videoAdHidden()
{
    if (m_musicPaused)
    {
        FMODAudioEngine::sharedEngine()->resumeAllMusic();
    }
    m_musicPaused = false;
}

void GameManager::applicationDidEnterBackground()
{
}

void GameManager::applicationWillEnterForeground()
{
    struct timespec ts;
    clock_gettime(0, &ts);
    double now = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    double elapsed = now - m_socialsDuration;
    
    if (elapsed < 4.0 && elapsed > 0.0 && m_socialsDuration != 0.0)
    {
        return;
    }
    
    if (m_clickedFacebook)
    {
        setUGV("23", true);
        reportAchievementWithID("geometry.ach.facebook", 100, false);
    }
    
    if (m_clickedYouTube)
    {
        setUGV("22", true);
        reportAchievementWithID("geometry.ach.youtube", 100, false);
    }
    
    if (m_clickedTwitter)
    {
        setUGV("24", true);
        reportAchievementWithID("geometry.ach.twitter", 100, false);
    }
    
    if (m_clickedTwitch)
    {
        setUGV("26", true);
    }
    
    if (m_clickedDiscord)
    {
        setUGV("27", true);
    }
    
    if (m_clickedReddit)
    {
        setUGV("32", true);
    }
    
    if (m_gameRateDelegate1)
    {
        m_gameRateDelegate1->updateRate();
    }
    if (m_gameRateDelegate2)
    {
        m_gameRateDelegate2->updateRate();
    }
    
    m_shouldResetShader = true;
    
    if (m_playLayer && m_playLayer->m_shaderLayer)
    {
        m_playLayer->m_shaderLayer->setupShader(true);
    }
    else if (m_levelEditorLayer && m_levelEditorLayer->m_shaderLayer)
    {
        m_levelEditorLayer->m_shaderLayer->setupShader(true);
    }
}

void GameManager::didExitPlayscene()
{
    if (m_unkBool8)
    {
        m_unkBool8 = false;
        if (cocos2d::CCDirector::sharedDirector()->m_nSmoothFixCounter > 9)
        {
            setGameVariable("0023", false);
        }
    }
}

void GameManager::startUpdate()
{
    cocos2d::CCScheduler* scheduler = cocos2d::CCDirector::sharedDirector()->getScheduler();
    scheduler->scheduleSelector(40, 1, this, 4294967294LL, 0, 0.0f, 0.0f);
}

void GameManager::loadVideoSettings()
{
    bool vsync = getGameVariable("0030");
    PlatformToolbox::toggleVerticalSync(vsync);
    
    bool forceTimer = getGameVariable("0032");
    PlatformToolbox::toggleForceTimer(forceTimer);
    
    bool smoothFix = getGameVariable("0023");
    PlatformToolbox::toggleSmoothFix(smoothFix);
}

void GameManager::updateCustomFPS()
{
    if (getGameVariable("0116"))
    {
        if (m_customFPSTarget > 60.0f)
        {
            cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1.0f / m_customFPSTarget);
        }
        else
        {
            cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1.0f / 60.0f);
        }
    }
}

GameManager::~GameManager()
{
}