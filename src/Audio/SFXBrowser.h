#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/SetTextPopupDelegate.h"
#include "Delegates/SelectSFXSortDelegate.h"
#include "Delegates/MusicDownloadDelegate.h"

class SFXBrowserDelegate;
class SFXSearchResult;
class SFXFolderObject;
class SFXInfoObject;
class GJCommentListLayer;
class CCMenuItemSpriteExtra;
class LoadingCircleSprite;

class SFXBrowser : public FLAlertLayer, public SetTextPopupDelegate, public SelectSFXSortDelegate, public MusicDownloadDelegate {
public:
    static SFXBrowser* create(int id);
    bool init(int id);

    int getSelectedCellIdx();
    void keyBackClicked() override;
    void musicActionFailed(GJMusicAction action) override;
    void musicActionFinished(GJMusicAction action) override;
    void onClearSearch(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onCredits(cocos2d::CCObject* sender);
    void onExitFolder(cocos2d::CCObject* sender);
    void onPage(cocos2d::CCObject* sender);
    void onSearch(cocos2d::CCObject* sender);
    void onSorting(cocos2d::CCObject* sender);
    void onToggleCompactMode(cocos2d::CCObject* sender);
    void onUpdateLibrary(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void setTextPopupClosed(SetTextPopup* popup, std::string text) override;
    void setupList(SFXSearchResult* searchResult);
    void setupSFXBrowser();
    bool shouldSnapToSelected();
    void sortSelectClosed(SelectSFXSortLayer* layer) override;
    void trySetupSFXBrowser();
    void updatePageLabel();

    ~SFXBrowser();

protected:
    int m_sfxID = 0;
    bool m_snapToSelected = false;
    SFXBrowserDelegate* m_delegate = nullptr;
    bool m_autoUpdating = false;
    bool m_gettingURL = false;
    LoadingCircleSprite* m_circleSprite = nullptr;
    SFXSearchResult* m_searchResult = nullptr;
    int m_cellAmount = 20;
    cocos2d::CCLabelBMFont* m_pageLabel = nullptr;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    int m_libraryVersion = 0;
    cocos2d::CCLabelBMFont* m_infoLabel = nullptr;
    cocos2d::CCArray* m_sfxObjects = nullptr;
    GJCommentListLayer* m_listLayer = nullptr;
    CCMenuItemSpriteExtra* m_clearSearchButton = nullptr;
    CCMenuItemSpriteExtra* m_backButton = nullptr;
    cocos2d::CCLabelBMFont* m_searchLabel = nullptr;
    cocos2d::CCLabelBMFont* m_nameLabel = nullptr;
    cocos2d::CCLabelBMFont* m_pathLabel = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    CCMenuItemSpriteExtra* m_refreshButton = nullptr;
    AudioSortType m_sortType = AudioSortType::Default;
    bool m_compactMode = false;
};