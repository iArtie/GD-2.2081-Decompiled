#pragma once

#include "cocos2d.h"
#include <string>
#include <map>
#include <vector>

class DS_Dictionary;
class GJGameLevel;
class GJBaseGameLayer;
class MenuLayer;
class PlayLayer;
class LevelEditorLayer;
class RewardedVideoDelegate;
class GameRateDelegate;
class FLAlertLayer;

enum IconType
{
    Cube = 0,
    Ship = 1,
    Ball = 2,
    Ufo = 3,
    Wave = 4,
    Robot = 5,
    Spider = 6,
    Swing = 7,
    Jetpack = 8,
    DeathEffect = 9,
    Special = 10,
    ShipFire = 11
};

enum UnlockType
{
    Col1 = 0,
    Col2 = 1,
    CubeType = 2,
    ShipType = 3,
    BallType = 4,
    UfoType = 5,
    WaveType = 6,
    RobotType = 7,
    SpiderType = 8,
    SwingType = 9,
    JetpackType = 10,
    DeathType = 11,
    StreakType = 12,
    ShipFireType = 13
};

struct RandomValue
{
    int seed;
    int random;
    int value;
};

struct UIButtonConfig
{
    int m_width;
    int m_height;
    float m_scale;
    int m_opacity;
    bool m_oneButton;
    bool m_player2;
    bool m_modeB;
    bool m_snap;
    bool m_split;
    cocos2d::CCPoint m_position;
    float m_deadzone;
    float m_radius;
};

class GameManager : public cocos2d::CCNode
{
public:
    static GameManager* sharedState();
    
    RandomValue m_playerUserID;
    RandomValue m_hasRP;
    bool m_hasDRP;
    
    bool init();
    void setup();
    void firstLoad();
    void dataLoaded(DS_Dictionary* dict);
    void encodeDataTo(DS_Dictionary* dict);
    void doQuickSave();
    void loadVideoSettings();
    void updateCustomFPS();
    void updateMusic();
    void resumeAudio();
    void resumeAudioDelayed();
    
    bool getGameVariable(const char* key);
    void setGameVariable(const char* key, bool value);
    int getIntGameVariable(const char* key);
    void setIntGameVariable(const char* key, int value);
    bool getUGV(const char* key);
    void setUGV(const char* key, bool value);
    bool toggleGameVariable(const char* key);
    
    void accountStatusChanged();
    void checkUsedIcons();
    void resetAllIcons();
    void resetCoinUnlocks();
    void resetDPadSettings(bool dual);
    void prepareDPadSettings();
    void loadDpadFromString(UIButtonConfig* config, std::string str);
    std::string dpadConfigToString(UIButtonConfig* config);
    void loadDPadLayout(int index, bool dual);
    void saveDPadLayout(int index, bool dual);
    
    void loadBackground(int index);
    void loadBackgroundAsync(int index);
    void loadGround(int index);
    void loadGroundAsync(int index);
    void loadMiddleground(int index);
    void loadMiddlegroundAsync(int index);
    void loadFont(int index);
    void loadDeathEffect(int id);
    void loadIconAsync(int id, int type, int requestID, cocos2d::CCObject* delegate);
    cocos2d::CCTexture2D* loadIcon(int id, int type, int requestID);
    void unloadIcon(int id, int type, int requestID);
    void unloadIcons(int requestID);
    void unloadBackground();
    void finishedLoadingBGAsync(cocos2d::CCObject* obj);
    void finishedLoadingGAsync1(cocos2d::CCObject* obj);
    void finishedLoadingGAsync2(cocos2d::CCObject* obj);
    void finishedLoadingMGAsync1(cocos2d::CCObject* obj);
    void finishedLoadingMGAsync2(cocos2d::CCObject* obj);
    void finishedLoadingGAsync(int index);
    void finishedLoadingMGAsync(int index);
    void finishedLoadingIconAsync(cocos2d::CCObject* obj);
    void logLoadedIconInfo();
    
    bool isIconUnlocked(int id, IconType type);
    bool isColorUnlocked(int id, UnlockType type);
    void unlockIcon(int id, IconType type);
    void unlockColor(int id, UnlockType type);
    void lockIcon(int id, IconType type);
    void lockColor(int id, UnlockType type);
    int activeIconForType(IconType type);
    IconType unlockTypeToIconType(int type);
    UnlockType iconTypeToUnlockType(IconType type);
    int countForType(IconType type);
    std::string iconKey(int id, IconType type);
    std::string colorKey(int id, UnlockType type);
    std::string sheetNameForIcon(int id, int type);
    int keyForIcon(int id, int type);
    void iconAndTypeForKey(int key, int* id, int* type);
    void calculateBaseKeyForIcons();
    int getIconRequestID();
    bool isIconLoaded(int id, int type);
    void addIconDelegate(cocos2d::CCObject* delegate, int key);
    void removeIconDelegate(int requestID);
    
    void addGameAnimation(int objectID, int frames, float frameTime, const std::string& mainFrame, const std::string& detailFrame, int defaultFrame);
    void addCustomAnimationFrame(int objectID, int frame, const std::string& mainFrame, const std::string& detailFrame);
    int framesForAnimation(int objectID);
    float frameTimeForAnimation(int objectID);
    int defaultFrameForAnimation(int objectID);
    void addDuplicateLastFrame(int objectID);
    void setupGameAnimations();
    
    void reloadAll(bool switchingModes, bool toFullscreen, bool unused);
    void reloadAllStep2();
    void reloadAllStep3();
    void reloadAllStep4();
    void reloadAllStep5();
    void reloadMenu();
    void queueReloadMenu();
    void returnToLastScene(GJGameLevel* level);
    bool safePopScene();
    void switchScreenMode(bool fullscreen, bool borderless, bool fix, bool unused);
    
    void playMenuMusic();
    void fadeInMenuMusic();
    void fadeInMusic(std::string path);
    std::string getMenuMusicFile();
    std::string getPracticeMusicFile();
    
    int getNextUniqueObjectKey();
    void addNewCustomObject(std::string str);
    void removeCustomObject(int key);
    std::string stringForCustomObject(int customObjectID);
    void switchCustomObjects(int key1, int key2);
    int reorderKey(int index, bool up);
    int getNextUsedKey(int index, bool up);
    cocos2d::CCArray* getOrderedCustomObjectKeys();
    
    void followTwitter();
    void followTwitch();
    void joinDiscord();
    void joinReddit();
    void likeFacebook();
    void subYouTube();
    void openEditorGuide();
    void rateGame();
    int generateSecretNumber();
    void unlockedPremium();
    void eventUnlockFeature(const char* key);
    void itemPurchased(const char* key);
    void claimItemsResponse(std::string str);
    void checkSteamAchievementUnlock();
    
    void verifyAchievementUnlocks();
    void verifyCoinUnlocks();
    void verifyStarUnlocks();
    void verifySyncedCoins();
    void recountUserStats(std::string str);
    void syncPlatformAchievements();
    void completedAchievement(std::string key);
    void getUnlockForAchievement(std::string key, int* id, UnlockType* type);
    void reportAchievementWithID(const char* key, int percent, bool dontNotify);
    void resetAchievement(std::string key);
    void reportPercentageForLevel(int levelID, int percentage, bool isPlatformer);
    
    bool levelIsPremium(int unk1, int unk2);
    bool groundHasSecondaryColor(int index);
    const char* getBGTexture(int index);
    const char* getGTexture(int index);
    const char* getMGTexture(int index);
    const char* getFontFile(int index);
    const char* getFontTexture(int index);
    cocos2d::CCSize resolutionForKey(int key);
    cocos2d::ccColor3B colorForIdx(int index);
    int colorForPos(int pos);
    
    void addToGJLog(cocos2d::CCString* str);
    void clearGJLog();
    void printGJLog();
    
    bool canShowRewardedVideo();
    void tryCacheAd();
    void saveAdTimer();
    void resetAdTimer();
    void shortenAdTimer(float time);
    bool shouldShowInterstitial(int unk1, int unk2, int unk3);
    bool showInterstitial();
    bool showInterstitialForced();
    bool showMainMenuAd();
    void tryShowInterstitial(int unk1, int unk2, int unk3);
    void rewardedVideoAdFinished(int unused);
    void rewardedVideoHidden();
    void rewardedVideoHiddenDelayed();
    void videoAdShowed();
    void videoAdHidden();
    
    void applicationDidEnterBackground();
    void applicationWillEnterForeground();
    void didExitPlayscene();
    void startUpdate();
    
    ~GameManager();

protected:
    cocos2d::CCActionManager* m_pActionManager;
    void* m_loadingBG;
    GJBaseGameLayer* m_gameLayer;
    int m_loadedBgID;
    void* m_switchModes;
    void* m_toFullscreen;
    void* m_reloading;
    void* m_fix;
    void* m_reloadTextures;
    void* m_unk664;
    RewardedVideoDelegate* m_rewardedVideoDelegate;
    int m_customMenuSongID;
    int m_customPracticeSongID;
    void* m_size;
    float m_adTimer;
    float m_adCache;
    bool m_musicPaused;
    float m_bgVolume;
    float m_sfxVolume;
    float m_timeOffset;
    bool m_ratedGame;
    bool m_hasRatedGame;
    bool m_clickedFacebook;
    double m_socialsDuration;
    bool m_clickedTwitter;
    bool m_clickedYouTube;
    bool m_clickedTwitch;
    bool m_clickedDiscord;
    bool m_clickedReddit;
    bool m_showedEditorGuide;
    bool m_googlePlaySignedIn;
    RandomValue m_playerFrame;
    RandomValue m_playerShip;
    RandomValue m_playerBall;
    RandomValue m_playerBird;
    RandomValue m_playerDart;
    RandomValue m_playerRobot;
    RandomValue m_playerSpider;
    RandomValue m_playerSwing;
    RandomValue m_playerJetpack;
    RandomValue m_playerDeathEffect;
    RandomValue m_playerStreak;
    RandomValue m_playerShipFire;
    std::map<int, std::vector<cocos2d::CCObject*>> m_iconDelegates;
    std::vector<int> m_keyStartForIcon;
    int m_iconRequestID;
    int m_loadedFont;
    int m_loadedDeathEffect;
    int m_loadedMG;
    void* m_loadingG1;
    bool m_finishedLoadingMG1;
    bool m_finishedLoadingMG2;
    int m_loadedGroundID;
    void* m_loadingG;
    bool m_finishedLoadingG1;
    bool m_finishedLoadingG2;
    cocos2d::CCDictionary* m_valueKeeper;
    bool m_unkBool8;
    cocos2d::CCDictionary* m_unlockValueKeeper;
    cocos2d::CCDictionary* m_customObjectDict;
    MenuLayer* m_menuLayer;
    RandomValue m_playerColor;
    RandomValue m_playerColor2;
    RandomValue m_playerGlowColor;
    bool m_playerGlow;
    int m_playerIconType;
    cocos2d::CCArray* m_gjLog;
    UIButtonConfig m_dpad1;
    UIButtonConfig m_dpad2;
    UIButtonConfig m_dpad3;
    UIButtonConfig m_dpad4;
    UIButtonConfig m_dpad5;
    bool m_quickSave;
    std::map<int, int> m_iconLoadCounts;
    std::string m_dpadLayoutDual1;
    std::string m_dpadLayoutDual2;
    std::string m_dpadLayoutDual3;
    std::string m_dpadLayout1;
    std::string m_dpadLayout2;
    std::string m_dpadLayout3;
    bool m_shouldLoadUnlockValueKeeper;
    std::string m_playerUDID;
    std::string m_playerName;
    bool m_firstSetup;
    bool m_showedMenu;
    bool m_unknownBool4;
    int m_resolution;
    bool m_showSongMarkers;
    bool m_showBPMMarkers;
    bool m_showProgressBar;
    bool m_performanceMode;
    bool m_vsyncEnabled;
    bool m_showedRateDiffDialog;
    bool m_showedRateStarDialog;
    bool m_showedLowDetailDialog;
    bool m_recordGameplay;
    bool m_loaded;
    int m_sceneEnum;
    bool m_searchObjectBool;
    float m_customFPSTarget;
    std::string m_fileName;
    bool m_unkBool2;
    void* m_unkDouble2;
    GameRateDelegate* m_gameRateDelegate1;
    GameRateDelegate* m_gameRateDelegate2;
    bool m_shouldResetShader;
    RandomValue m_secretNumber;
    bool m_disableThumbstick;
    bool m_clickedEditor;
    bool m_clickedGarage;
    bool m_clickedName;
    bool m_clickedPractice;
    int m_bootups;
    int m_texQuality;
    float m_practiceOpacity;
    cocos2d::CCPoint m_practicePos;
    std::map<int, std::map<int, int>> m_iconRequests;
    std::map<int, bool> m_isIconBeingLoaded;
    cocos2d::CCDictionary* m_mainFramesForAnimation;
    cocos2d::CCDictionary* m_detailFramesForAnimation;
    cocos2d::CCDictionary* m_frameTimeForAnimation;
    cocos2d::CCDictionary* m_framesForAnimation;
    cocos2d::CCDictionary* m_defaultFrames;
    PlayLayer* m_playLayer;
    LevelEditorLayer* m_levelEditorLayer;
    RandomValue m_chk;
};