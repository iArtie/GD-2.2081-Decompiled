#pragma once

#include "Platform/cocos2d.h"
#include <string>

#include "Layers/FLAlertLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/CommentUploadDelegate.h"
#include "Delegates/UploadPopupDelegate.h"
#include "Delegates/UploadActionDelegate.h"
#include "Delegates/UserInfoDelegate.h"
#include "Delegates/LeaderboardManagerDelegate.h"
#include "Delegates/LevelCommentDelegate.h"

class LoadingCircle;
class CCMenuItemSpriteExtra;
class GJCommentListLayer;
class GJUserScore;
class UploadActionPopup;

class ProfilePage : public FLAlertLayer,
                    public CommentUploadDelegate,
                    public UploadPopupDelegate,
                    public UploadActionDelegate,
                    public UserInfoDelegate,
                    public LeaderboardManagerDelegate,
                    public LevelCommentDelegate
{
public:
    static ProfilePage* create(int accountID, bool ownProfile);

    bool init(int accountID, bool ownProfile);
    void registerWithTouchDispatcher() override;
    void keyBackClicked() override;
    void show() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;

    void blockUser();

    void commentDeleteFailed(int id, int parentID) override;
    void commentUploadFailed(int parentID, CommentError errorType) override;
    void commentUploadFinished(int parentID) override;

    void getUserInfoFailed(int id) override;
    void getUserInfoFinished(GJUserScore* score) override;
    void userInfoChanged(GJUserScore* score) override;

    void updateUserScoreFailed() override;
    void updateUserScoreFinished() override;

    void loadCommentsFailed(const char* key) override;
    void loadCommentsFinished(cocos2d::CCArray* comments, const char* key) override;
    void setupPageInfo(std::string info, const char* key) override;

    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;

    void onClosePopup(UploadActionPopup* popup) override;

    bool isCorrect(const char* key);
    bool isOnWatchlist(int id);

    void loadPage(int page);
    void loadPageFromUserInfo(GJUserScore* score);

    void onBlockUser(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onComment(cocos2d::CCObject* sender);
    void onCommentHistory(cocos2d::CCObject* sender);
    void onCopyName(cocos2d::CCObject* sender);
    void onFollow(cocos2d::CCObject* sender);
    void onFriend(cocos2d::CCObject* sender);
    void onFriends(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onMessages(cocos2d::CCObject* sender);
    void onMyLevels(cocos2d::CCObject* sender);
    void onMyLists(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onRequests(cocos2d::CCObject* sender);
    void onSendMessage(cocos2d::CCObject* sender);
    void onSettings(cocos2d::CCObject* sender);
    void onSocialPage(cocos2d::CCObject* sender);
    void onStatInfo(cocos2d::CCObject* sender);
    void onUpdate(cocos2d::CCObject* sender);

    void setupComments();
    void setupCommentsBrowser(cocos2d::CCArray* comments);
    void showNoAccountError();
    void toggleMainPageVisibility(bool visible);
    void toggleShip(cocos2d::CCObject* sender);
    void updateLevelsLabel();
    void updatePageArrows();

    virtual ~ProfilePage();

protected:
    LoadingCircle* m_circle = nullptr;
    CCMenuItemSpriteExtra* m_refreshBtn = nullptr;
    cocos2d::CCLabelBMFont* m_somethingWentWrong = nullptr;
    GJCommentListLayer* m_list = nullptr;
    GJUserScore* m_score = nullptr;
    UploadActionPopup* m_popupDelegate = nullptr;
    int m_accountID = 0;
    CCMenuItemSpriteExtra* m_followBtn = nullptr;
    bool m_ownProfile = false;
    cocos2d::CCArray* m_buttons = nullptr;
    cocos2d::CCLabelBMFont* m_usernameLabel = nullptr;
    CCMenuItemSpriteExtra* m_leftArrow = nullptr;
    CCMenuItemSpriteExtra* m_rightArrow = nullptr;
    std::string m_profileKey;
    cocos2d::CCArray* m_arrayWithUsernameLabel = nullptr;
    int m_itemCount = 0;
    int m_pageStartIdx = 0;
    int m_pageEndIdx = 0;
    int m_page = 0;
};