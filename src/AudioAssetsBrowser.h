#pragma once

#include "cocos2d.h"
#include <vector>

class LoadingCircleSprite;
class SongInfoObject;
class CustomListView;
class GJCommentListLayer;
class MusicDownloadManager;

class AudioAssetsBrowser {
public:
    static AudioAssetsBrowser* create(std::vector<int>* songIds, std::vector<int>* sfxIds);
    int getCellDelegateType();
    int getSelectedCellIdx();
    bool init(std::vector<int>* songIds, std::vector<int>* sfxIds);
    void keyBackClicked();
    void musicActionFailed(int action);
    void onClose(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onPage(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher();
    void setupList();
    void trySetupAudioBrowser();
    void updatePageLabel();
    ~AudioAssetsBrowser();

private:
    LoadingCircleSprite* m_loadingCircle;
    std::vector<int> m_songIds;
    std::vector<int> m_sfxIds;
    cocos2d::CCArray* m_songInfoObjects;
    GJCommentListLayer* m_songList;
    cocos2d::CCLayer* m_mainLayer;
    bool m_noElasticity;
    cocos2d::CCMenu* m_buttonMenu;
    int m_joystickConnected;
    cocos2d::CCLabelBMFont* m_pageIndicatorLabel;
    int m_controlConnected;
    int m_alertProtocol;
    int m_scene;
    int m_reverseKeyBack;
    int m_ZOrder;
    int m_button1;
    int m_button2;
    int m_scrollingLayer;
    int m_containsBorder;
    int m_noAction;
    int m_forcePrioRegistered;
};