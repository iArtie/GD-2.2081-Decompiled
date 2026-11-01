#pragma once

#include "FLAlertLayer.h"
#include "UserListDelegate.h"
#include "cocos2d.h"

enum UserListType {
    Friends = 0,
    Blocked = 1
};

enum GJErrorCode;

class GJCommentListLayer;
class LoadingCircle;
class CCMenuItemSpriteExtra;

class FriendsProfilePage : public FLAlertLayer, public UserListDelegate {
public:
    static FriendsProfilePage* create(UserListType type);
    void forceReloadList(UserListType type) override;
    void getUserListFailed(UserListType type, GJErrorCode errorType) override;
    void getUserListFinished(cocos2d::CCArray* scores, UserListType type) override;
    bool init(UserListType type) override;
    void keyBackClicked() override;
    void onBlocked(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onUpdate(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void setupUsersBrowser(cocos2d::CCArray* users, UserListType type);
    void userListChanged(cocos2d::CCArray* scores, UserListType type) override;
    
    virtual ~FriendsProfilePage();

private:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLabelBMFont* m_totalFriends;
    cocos2d::CCLabelBMFont* m_noInternet;
    GJCommentListLayer* m_listLayer;
    LoadingCircle* m_circle;
    CCMenuItemSpriteExtra* m_refreshBtn;
    cocos2d::CCArray* m_users;
    UserListType m_type;
    bool m_noElasticity;
};