#pragma once

#include "cocos2d.h"
#include "FLAlertLayerProtocol.h"
#include "CustomSFXDelegate.h"
#include "SFXInfoObject.h"
#include "MusicDownloadManager.h"

class CustomSFXWidget : public cocos2d::CCNode, public FLAlertLayerProtocol {
public:
    static CustomSFXWidget* create(SFXInfoObject* object, CustomSFXDelegate* delegate, bool showPlay, bool showDownload, bool showDelete, int bgColor, bool compactMode);
    bool init(SFXInfoObject* object, CustomSFXDelegate* delegate, bool showPlay, bool showDownload, bool showDelete, int bgColor, bool compactMode);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void deleteSFX();
    void downloadFailed();
    void downloadSFXFailed(int id, GJSongError errorType);
    void downloadSFXFinished(int id);
    void hideLoadingArt();
    void onCancelDownload(cocos2d::CCObject* sender);
    void onDelete(cocos2d::CCObject* sender);
    void onDownload(cocos2d::CCObject* sender);
    void onPlayback(cocos2d::CCObject* sender);
    void onSelect(cocos2d::CCObject* sender);
    void showLoadingArt();
    void songStateChanged();
    void startDownload();
    void startMonitorDownload();
    void updateDownloadProgress(float dt);
    void updateError(GJSongError errorType);
    void updateLengthMod(float mod);
    void updatePlaybackBtn();
    void updateProgressBar(int progress);
    void updateSFXInfo();
    void updateSFXObject(SFXInfoObject* object);
    bool verifySFXID(int id);
    
    virtual ~CustomSFXWidget();

protected:
    SFXInfoObject* m_sfxObject;
    CustomSFXDelegate* m_delegate;
    int m_sfxID;
    bool m_compactMode;
    bool m_showPlay;
    bool m_showDownload;
    bool m_showDelete;
    bool m_showCancel;
    float m_lengthMod;
    
    cocos2d::CCSprite* m_clockSprite;
    cocos2d::CCSprite* m_progressOutlineSprite;
    cocos2d::CCSprite* m_progressBarSprite;
    cocos2d::CCLabelBMFont* m_idLabel;
    cocos2d::CCLabelBMFont* m_titleLabel;
    cocos2d::CCLabelBMFont* m_errorLabel;
    
    CCMenuItemSpriteExtra* m_downloadButton;
    CCMenuItemSpriteExtra* m_cancelButton;
    CCMenuItemSpriteExtra* m_deleteButton;
    CCMenuItemSpriteExtra* m_selectButton;
    CCMenuItemSpriteExtra* m_playButton;
};