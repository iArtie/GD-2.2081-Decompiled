#pragma once

#include "Layers/FLAlertLayer.h"
#include "Delegates/FriendRequestDelegate.h"
#include "Delegates/UploadActionDelegate.h"
#include "Delegates/UploadPopupDelegate.h"
#include "Platform/cocos2d.h"
#include <string>

class GJCommentListLayer;
class LoadingCircle;
class UploadActionPopup;
class CCMenuItemSpriteExtra;

class FRequestProfilePage : public FLAlertLayer, 
                            public FriendRequestDelegate, 
                            public UploadActionDelegate, 
                            public UploadPopupDelegate {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    static FRequestProfilePage* create(bool sent);
    void deleteSelected();
    void forceReloadRequests(bool sent);
    bool init(bool sent) override;
    bool isCorrect(const char* key);
    void keyBackClicked() override;
    void loadFRequestsFailed(const char* key, GJErrorCode errorType) override;
    void loadFRequestsFinished(cocos2d::CCArray* scores, const char* key) override;
    void loadPage(int page);
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup) override;
    void onDeleteSelected(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onSentRequests(cocos2d::CCObject* sender);
    void onToggleAllObjects(cocos2d::CCObject* sender);
    void onUpdate(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void setupCommentsBrowser(cocos2d::CCArray* scores);
    void setupPageInfo(std::string info, const char* key);
    void untoggleAll();
    void updateLevelsLabel();
    void updatePageArrows();
    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;
    
    virtual ~FRequestProfilePage();

private:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLabelBMFont* m_pageLabel;
    cocos2d::CCLabelBMFont* m_errorLabel;
    GJCommentListLayer* m_listLayer;
    LoadingCircle* m_circle;
    UploadActionPopup* m_uploadPopup;
    CCMenuItemSpriteExtra* m_refreshButton;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    std::string m_key;
    int m_itemCount;
    int m_page;
    int m_pageStartIdx;
    int m_pageEndIdx;
    bool m_sent;
    bool m_noElasticity;
    bool m_toggledAll;
};