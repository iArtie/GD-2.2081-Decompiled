#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include <string>

class FLAlertLayer;
class GJCommentListLayer;
class MusicBrowserDelegate;
class MusicSearchResult;
class Slider;
class LoadingCircleSprite;
class CCMenuItemSpriteExtra;
class SetTextPopup;

class MusicBrowser : public FLAlertLayer,
                     public FLAlertLayerProtocol,
                     public MusicDownloadDelegate,
                     public SetTextPopupDelegate,
                     public SliderDelegate {
public:
    static MusicBrowser* create(int songID, GJSongType songType);

    bool init(int songID, GJSongType songType);
    void keyBackClicked() override;
    void registerWithTouchDispatcher() override;
    void update(float dt) override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    int getSelectedCellIdx();
    void musicActionFailed(GJMusicAction action);
    void musicActionFinished(GJMusicAction action);
    void onArtistFilters(cocos2d::CCObject* sender);
    void onClearSearch(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onPage(cocos2d::CCObject* sender);
    void onPlaybackControl(cocos2d::CCObject* sender);
    void onSearch(cocos2d::CCObject* sender);
    void onTagFilters(cocos2d::CCObject* sender);
    void onUpdateLibrary(cocos2d::CCObject* sender);
    void setTextPopupClosed(SetTextPopup* popup, std::string text);
    void setupList(MusicSearchResult* musicSearchResult);
    void setupMusicBrowser();
    void setupSongControls();
    void sliderChanged(cocos2d::CCObject* sender);
    void sliderEnded(Slider* slider);
    void trySetupMusicBrowser();
    void updatePageLabel();

    virtual ~MusicBrowser();

protected:
    int m_songID = 0;
    GJCommentListLayer* m_listLayer = nullptr;
    MusicBrowserDelegate* m_delegate = nullptr;
    Slider* m_playSlider = nullptr;
    cocos2d::CCLabelBMFont* m_playLabel = nullptr;
    CCMenuItemSpriteExtra* m_playButton = nullptr;
    bool m_autoUpdating = false;
    bool m_gettingURL = false;
    LoadingCircleSprite* m_circleSprite = nullptr;
    MusicSearchResult* m_searchResult = nullptr;
    cocos2d::CCLabelBMFont* m_pageLabel = nullptr;
    int m_libraryVersion = 0;
    GJSongType m_songType = GJSongType::Normal;
    cocos2d::CCLabelBMFont* m_infoLabel = nullptr;
    CCMenuItemSpriteExtra* m_refreshButton = nullptr;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    CCMenuItemSpriteExtra* m_clearSearchButton = nullptr;
    CCMenuItemSpriteExtra* m_tagFilterButton = nullptr;
    CCMenuItemSpriteExtra* m_artistFilterButton = nullptr;
};