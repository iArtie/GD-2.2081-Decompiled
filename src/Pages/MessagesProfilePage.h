#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include <string>

class FLAlertLayer;
class GJCommentListLayer;
class UploadActionPopup;
class CCMenuItemToggler;
class LoadingCircle;
class CCMenuItemSpriteExtra;

class MessagesProfilePage : public FLAlertLayer,
                            public FLAlertLayerProtocol,
                            public MessageListDelegate,
                            public UploadActionDelegate,
                            public UploadPopupDelegate {
public:
    static MessagesProfilePage* create(bool sent);

    bool init(bool sent);
    void keyBackClicked() override;
    void registerWithTouchDispatcher() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void deleteSelected();
    void forceReloadMessages(bool sent);
    bool isCorrect(const char* key);
    void loadMessagesFailed(const char* key, GJErrorCode errorType);
    void loadMessagesFinished(cocos2d::CCArray* messages, const char* key);
    void loadPage(int page);
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup);
    void onDeleteSelected(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onSentMessages(cocos2d::CCObject* sender);
    void onToggleAllObjects(cocos2d::CCObject* sender);
    void onUpdate(cocos2d::CCObject* sender);
    void setupCommentsBrowser(cocos2d::CCArray* messages);
    void setupPageInfo(std::string info, const char* key);
    void untoggleAll();
    void updateLevelsLabel();
    void updatePageArrows();
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);

    virtual ~MessagesProfilePage();

protected:
    bool m_toggledAll = false;
    std::string m_messageKey;
    GJCommentListLayer* m_listLayer = nullptr;
    UploadActionPopup* m_actionPopup = nullptr;
    bool m_sentMessages = false;
    CCMenuItemToggler* m_toggleAllToggler = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    CCMenuItemSpriteExtra* m_refreshButton = nullptr;
    cocos2d::CCLabelBMFont* m_errorLabel = nullptr;
    cocos2d::CCMenu* m_buttonMenu = nullptr;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    unsigned int m_itemCount = 0;
    int m_page = 0;
    cocos2d::CCLabelBMFont* m_levelsLabel = nullptr;
    int m_pageStartIdx = 0;
    int m_pageEndIdx = 0;
};