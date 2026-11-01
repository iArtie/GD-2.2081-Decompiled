#pragma once

#include "cocos2d.h"
#include "extension/CCScale9Sprite.h"
#include "FLAlertLayerProtocol.h"
#include "CustomSongDelegate.h"
#include "SongPlaybackDelegate.h"
#include "SongInfoObject.h"
#include "MusicDownloadManager.h"
#include <set>
#include <vector>

enum GJAssetType {
    Song = 1,
    SFX = 2
};

enum GJMusicAction {
    DownloadOrUpdate = 0
};

struct GJAssetDownloadAction {
    int m_id;
    GJAssetType m_assetType;
    int m_status;
    bool m_isDownloading;
};

class CustomSongWidget : public cocos2d::CCNode, public FLAlertLayerProtocol {
public:
    static CustomSongWidget* create(SongInfoObject* songInfo, CustomSongDelegate* delegate, bool showPlay, bool showDownload, bool showMore, int bgColor, int textColor, bool showSize, bool isLibrary);
    bool init(SongInfoObject* songInfo, CustomSongDelegate* delegate, bool showPlay, bool showDownload, bool showMore, int bgColor, int textColor, bool showSize, bool isLibrary);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void addExtraVisuals(bool newMusic, bool newLibraryMusic);
    void deleteSong();
    void downloadAssetFailed(int id, GJAssetType type, GJSongError errorType);
    void downloadAssetFinished(int id, GJAssetType type);
    void downloadFailed();
    void downloadSFXFailed(int id, GJSongError errorType);
    void downloadSFXFinished(int id);
    void downloadSongFailed(int id, GJSongError errorType);
    void downloadSongFinished(int id);
    void downloadSongStarted(int id);
    void getSongInfoIfUnloaded();
    void loadSongInfoFailed(int id, GJSongError errorType);
    void loadSongInfoFinished(SongInfoObject* object);
    void musicActionFinished(GJMusicAction action);
    void onCancelDownload(cocos2d::CCObject* sender);
    void onDelete(cocos2d::CCObject* sender);
    void onDownload(cocos2d::CCObject* sender);
    void onGetSongInfo(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onMore(cocos2d::CCObject* sender);
    void onPlayback(cocos2d::CCObject* sender);
    void onSelect(cocos2d::CCObject* sender);
    void positionInfoObjects();
    void processNextMultiAsset();
    void showError(bool fast);
    void songStateChanged();
    void startDownload();
    void startMonitorDownload();
    void startMultiAssetDownload();
    void toggleUpdateButton(bool enabled);
    void updateDownloadProgress(float dt);
    void updateError(GJSongError errorType);
    void updateLengthMod(float lengthMod);
    void updateMultiAssetInfo(bool update);
    void updatePlaybackBtn();
    void updateProgressBar(int progress);
    void updateSongInfo();
    void updateSongObject(SongInfoObject* songInfo);
    bool verifySongID(int id);
    
    virtual ~CustomSongWidget();

    bool m_isInCell;

protected:
    SongInfoObject* m_songInfoObject;
    CustomSongDelegate* m_songDelegate;
    SongPlaybackDelegate* m_playbackDelegate;
    
    int m_customSongID;
    bool m_isMusicLibrary;
    bool m_isRobtopSong;
    bool m_showDownloadBtn;
    bool m_showSelectSongBtn;
    bool m_showPlayMusicBtn;
    bool m_isNotDownloading;
    bool m_hasSFX;
    bool m_unkBool2;
    bool m_hasLibrarySongs;
    float m_lengthMod;
    int m_totalBytes;
    
    cocos2d::CCLabelBMFont* m_songLabel;
    cocos2d::CCLabelBMFont* m_artistLabel;
    cocos2d::CCLabelBMFont* m_songIDLabel;
    cocos2d::CCLabelBMFont* m_errorLabel;
    cocos2d::CCSprite* m_ncsLogo;
    cocos2d::CCSprite* m_sliderGroove;
    cocos2d::CCSprite* m_sliderBar;
    cocos2d::extension::CCScale9Sprite* m_bgSpr;
    
    CCMenuItemSpriteExtra* m_playbackBtn;
    CCMenuItemSpriteExtra* m_downloadBtn;
    CCMenuItemSpriteExtra* m_cancelDownloadBtn;
    CCMenuItemSpriteExtra* m_deleteBtn;
    CCMenuItemSpriteExtra* m_selectSongBtn;
    CCMenuItemSpriteExtra* m_moreBtn;
    CCMenuItemSpriteExtra* m_getSongInfoBtn;
    CCMenuItemSpriteExtra* m_infoBtn;
    
    std::set<int> m_songs;
    std::set<int> m_sfx;
    std::set<int> m_nextSongIDs;
    std::vector<GJAssetDownloadAction> m_undownloadedAssets;
};