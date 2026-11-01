#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/LevelDownloadDelegate.h"
#include "Delegates/LevelUpdateDelegate.h"
#include "Delegates/RateLevelDelegate.h"
#include "Delegates/LikeItemDelegate.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/LevelDeleteDelegate.h"
#include "Delegates/NumberInputDelegate.h"
#include "Delegates/SetIDPopupDelegate.h"
#include "Delegates/TableViewCellDelegate.h"
#include "Models/GJGameLevel.h"

class NumberInputLayer;
class SetIDPopup;
class FLAlertLayer;
class LoadingCircle;
class CustomSongWidget;
class CCMenuItemSpriteExtra;
class GJDifficultySprite;

class LevelInfoLayer : public cocos2d::CCLayer,
                       public LevelDownloadDelegate,
                       public LevelUpdateDelegate,
                       public RateLevelDelegate,
                       public LikeItemDelegate,
                       public FLAlertLayerProtocol,
                       public LevelDeleteDelegate,
                       public NumberInputDelegate,
                       public SetIDPopupDelegate,
                       public TableViewCellDelegate {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void confirmClone(cocos2d::CCObject* sender);
    void confirmDelete(cocos2d::CCObject* sender);
    void confirmMoveToBottom(cocos2d::CCObject* sender);
    void confirmMoveToTop(cocos2d::CCObject* sender);
    void confirmOwnerDelete(cocos2d::CCObject* sender);
    static LevelInfoLayer* create(GJGameLevel* level, bool challenge);
    void downloadLevel();
    void incrementDislikes();
    void incrementLikes();
    bool init(GJGameLevel* level, bool challenge);
    void keyBackClicked();
    void levelDeleteFailed(int id);
    void levelDeleteFinished(int id);
    void levelDownloadFailed(int response);
    void levelDownloadFinished(GJGameLevel* level);
    void levelUpdateFailed(int response);
    void levelUpdateFinished(GJGameLevel* level, UpdateResponse response);
    void likedItem(LikeItemType type, int id, bool liked);
    void loadLevelStep();
    void numberInputClosed(NumberInputLayer* layer);
    void onAddToList(cocos2d::CCObject* sender);
    void onBack(cocos2d::CCObject* sender);
    void onClone(cocos2d::CCObject* sender);
    void onDelete(cocos2d::CCObject* sender);
    void onEnterTransitionDidFinish();
    void onFavorite(cocos2d::CCObject* sender);
    void onGarage(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onLevelInfo(cocos2d::CCObject* sender);
    void onLevelLeaderboard(cocos2d::CCObject* sender);
    void onLevelOptions(cocos2d::CCObject* sender);
    void onLike(cocos2d::CCObject* sender);
    void onOwnerDelete(cocos2d::CCObject* sender);
    void onPlay(cocos2d::CCObject* sender);
    void onPlayReplay(cocos2d::CCObject* sender);
    void onRate(cocos2d::CCObject* sender);
    void onRateDemon(cocos2d::CCObject* sender);
    void onRateStars(cocos2d::CCObject* sender);
    void onRateStarsMod(cocos2d::CCObject* sender);
    void onSetFolder(cocos2d::CCObject* sender);
    void onUpdate(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void playStep2();
    void playStep3();
    void playStep4();
    static cocos2d::CCScene* scene(GJGameLevel* level, bool challenge);
    void setIDPopupClosed(SetIDPopup* popup, int value);
    void setupLevelInfo();
    void setupPlatformerStats();
    void setupProgressBars();
    bool shouldDownloadLevel();
    void showSongWarning();
    void showUpdateAlert(UpdateResponse type);
    void tryCloneLevel(cocos2d::CCObject* sender);
    void tryShowAd();
    void updateLabelValues();
    void updateSideButtons();
    ~LevelInfoLayer();

private:
    cocos2d::CCLabelBMFont* m_folderLabel = nullptr;
    GJGameLevel* m_level = nullptr;
    cocos2d::CCScene* m_playScene = nullptr;
    cocos2d::CCProgressTimer* m_progressTimer = nullptr;
    bool m_isBusy = false;
    LoadingCircle* m_circle = nullptr;
    cocos2d::CCMenu* m_playBtnMenu = nullptr;
    bool m_noUpdateAlert = false;
    bool m_enterTransitionFinished = false;
    cocos2d::CCSprite* m_playSprite = nullptr;
    CustomSongWidget* m_songWidget = nullptr;
    CCMenuItemSpriteExtra* m_likeBtn = nullptr;
    CCMenuItemSpriteExtra* m_starRateBtn = nullptr;
    CCMenuItemSpriteExtra* m_demonRateBtn = nullptr;
    cocos2d::CCLabelBMFont* m_exactLengthLabel = nullptr;
    cocos2d::CCLabelBMFont* m_lengthLabel = nullptr;
    cocos2d::CCLabelBMFont* m_downloadsLabel = nullptr;
    cocos2d::CCLabelBMFont* m_orbsLabel = nullptr;
    cocos2d::CCLabelBMFont* m_likesLabel = nullptr;
    cocos2d::CCSprite* m_likesIcon = nullptr;
    cocos2d::CCPoint m_basePosition;
    GJDifficultySprite* m_difficultySprite = nullptr;
    cocos2d::CCLabelBMFont* m_starsLabel = nullptr;
    cocos2d::CCSprite* m_starsIcon = nullptr;
    float m_baseStarsY = 0.0f;
    bool m_challenge = false;
    cocos2d::CCSprite* m_orbsIcon = nullptr;
    float m_iconOffset = 0.0f;
    cocos2d::CCArray* m_icons = nullptr;
    cocos2d::CCArray* m_coins = nullptr;
    GJLevelType m_levelType = GJLevelType::Local;
    CCMenuItemSpriteExtra* m_cloneBtn = nullptr;
};