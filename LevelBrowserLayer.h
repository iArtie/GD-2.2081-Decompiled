#pragma once

#include "cocos2d.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "GJSearchObject.h"
#include "GJListLayer.h"
#include "LoadingCircle.h"
#include "TextArea.h"
#include "FLAlertLayer.h"
#include "SetIDPopup.h"
#include "SetTextPopup.h"
#include "ShareCommentLayer.h"
#include "DialogDelegate.h"
#include <string>

class LevelBrowserLayer : public cocos2d::CCLayerColor, public FLAlertLayerProtocol, public SetIDPopupDelegate, public SetTextPopupDelegate, public ShareCommentDelegate {
public:
    static LevelBrowserLayer* create(GJSearchObject* object);
    static cocos2d::CCScene* scene(GJSearchObject* object);

    virtual bool init(GJSearchObject* object) override;
    virtual void onEnter() override;
    virtual void keyBackClicked() override;
    virtual void registerWithTouchDispatcher() override;
    virtual ~LevelBrowserLayer() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void setIDPopupClosed(SetIDPopup* popup, int value) override;
    void setTextPopupClosed(SetTextPopup* popup, std::string text) override;
    void shareCommentClosed(std::string text, ShareCommentLayer* layer) override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    void createNewLevel(cocos2d::CCObject* sender);
    void createNewList(cocos2d::CCObject* sender);
    void createNewSmartTemplate(cocos2d::CCObject* sender);
    void deleteSelected();
    void exitLayer(cocos2d::CCObject* sender);
    std::string getSearchTitle();
    bool isCorrect(const char* key);
    void loadLevelsFailed(const char* key, int type);
    void loadPage(GJSearchObject* object);
    void onBack(cocos2d::CCObject* sender);
    void onClearSearch(cocos2d::CCObject* sender);
    void onDeleteAll(cocos2d::CCObject* sender);
    void onDeleteSelected(cocos2d::CCObject* sender);
    void onFavorites(cocos2d::CCObject* sender);
    void onGoToFolder(cocos2d::CCObject* sender);
    void onGoToLastPage(cocos2d::CCObject* sender);
    void onGoToPage(cocos2d::CCObject* sender);
    void onHelp(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onLocalMode(cocos2d::CCObject* sender);
    void onMyOnlineLevels(cocos2d::CCObject* sender);
    void onNew(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onRefresh(cocos2d::CCObject* sender);
    void onRemoveAllFavorites(cocos2d::CCObject* sender);
    void onSaved(cocos2d::CCObject* sender);
    void onSavedMode(cocos2d::CCObject* sender);
    void onSearch(cocos2d::CCObject* sender);
    void onToggleAllObjects(cocos2d::CCObject* sender);
    void reloadAllObjects();
    void setSearchObject(GJSearchObject* object);
    void setupLevelBrowser(cocos2d::CCArray* items);
    void setupPageInfo(std::string info, const char* key);
    void show();
    void updateLevelsLabel();
    void updatePageLabel();
    cocos2d::CCArray* updateResultArray(cocos2d::CCArray* results);

protected:
    GJSearchObject* m_searchObject = nullptr;
    GJListLayer* m_list = nullptr;
    cocos2d::CCArray* m_levels = nullptr;
    LoadingCircle* m_circle = nullptr;
    TextArea* m_noInternet = nullptr;
    CCMenuItemSpriteExtra* m_refreshBtn = nullptr;
    CCMenuItemSpriteExtra* m_leftArrow = nullptr;
    CCMenuItemSpriteExtra* m_rightArrow = nullptr;
    CCMenuItemSpriteExtra* m_pageBtn = nullptr;
    CCMenuItemSpriteExtra* m_lastBtn = nullptr;
    CCMenuItemSpriteExtra* m_folderBtn = nullptr;
    CCMenuItemSpriteExtra* m_cancelSearchBtn = nullptr;
    CCMenuItemToggler* m_allObjectsToggler = nullptr;
    cocos2d::CCLabelBMFont* m_pageText = nullptr;
    cocos2d::CCLabelBMFont* m_folderText = nullptr;
    cocos2d::CCLabelBMFont* m_countText = nullptr;
    cocos2d::CCScene* m_scene = nullptr;
    bool m_isOverlay = false;
    bool m_allSelected = false;
    bool m_unk = false;
    bool m_unk2 = false;
    bool m_cached = false;
    int m_listHeight = 0;
    int m_lastPage = 0;
    int m_pageStartIdx = 0;
    int m_pageEndIdx = 0;
    int m_zOffset = 0;
    unsigned int m_itemCount = 0;
    float m_unkFloat = 0.0f;
};