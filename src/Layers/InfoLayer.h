#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJGameLevel;
class GJUserScore;
class GJLevelList;
class GJCommentListLayer;
class LoadingCircle;
class CCMenuItemSpriteExtra;
class FLAlertLayer;

class InfoLayer : public FLAlertLayer {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void commentUploadFailed(int parentID, CommentError errorType);
    void commentUploadFinished(int parentID);
    void confirmReport(cocos2d::CCObject* sender);
    static InfoLayer* create(GJGameLevel* level, GJUserScore* score, GJLevelList* list);
    int getAccountID();
    int getID();
    int getRealID();
    bool init(GJGameLevel* level, GJUserScore* score, GJLevelList* list) override;
    bool isCorrect(const char* key);
    void keyBackClicked() override;
    void loadCommentsFailed(const char* key);
    void loadCommentsFinished(cocos2d::CCArray* comments, const char* key);
    void loadPage(int page, bool noSetup);
    void onClose(cocos2d::CCObject* sender);
    void onComment(cocos2d::CCObject* sender);
    void onCopyLevelID(cocos2d::CCObject* sender);
    void onGetComments(cocos2d::CCObject* sender);
    void onLevelInfo(cocos2d::CCObject* sender);
    void onMore(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onOriginal(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onRefreshComments(cocos2d::CCObject* sender);
    void onSimilar(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void reloadWindow();
    void setupCommentsBrowser(cocos2d::CCArray* comments);
    void setupLevelInfo();
    void setupPageInfo(const std::string& info, const char* key);
    void show() override;
    void toggleCommentMode(cocos2d::CCObject* sender);
    void toggleExtendedMode(cocos2d::CCObject* sender);
    void toggleSmallCommentMode(cocos2d::CCObject* sender);
    void updateCommentModeButtons();
    void updateLevelsLabel();
    void updateUserScoreFinished();
    virtual ~InfoLayer();

private:
    GJCommentListLayer* m_list;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    GJGameLevel* m_level;
    cocos2d::CCMenu* m_buttonMenu;
    GJLevelList* m_levelList;
    GJUserScore* m_score;
    bool m_canUpdateUserScore;
    int m_page;
    unsigned int m_itemCount;
    int m_mode;
    CCMenuItemSpriteExtra* m_reportBtn;
    std::string m_commentKey;
    LoadingCircle* m_loadingCircle;
    cocos2d::CCLabelBMFont* m_noComments;
    CCMenuItemSpriteExtra* m_commentsBtn;
    CCMenuItemSpriteExtra* m_leftArrow;
    CCMenuItemSpriteExtra* m_rightArrow;
    cocos2d::CCLabelBMFont* m_pageLabel;
    int m_pageStartIdx;
    int m_pageEndIdx;
    CCMenuItemSpriteExtra* m_likeBtn;
    CCMenuItemSpriteExtra* m_timeBtn;
    CCMenuItemSpriteExtra* m_refreshCommentsBtn;
};