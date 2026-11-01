#pragma once

#include "Platform/cocos2d.h"

class GJUserScore;
class UploadActionPopup;
class FLAlertLayer;
class TableViewCell;

class GJUserCell : public TableViewCell {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    GJUserCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromScore(GJUserScore* score);
    void onCancelFriendRequest(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup);
    void onRemoveFriend(cocos2d::CCObject* sender);
    void onSendMessage(cocos2d::CCObject* sender);
    void onUnblockUser(cocos2d::CCObject* sender);
    void onViewFriendRequest(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);
    virtual ~GJUserCell();

private:
    GJUserScore* m_userScore;
    UploadActionPopup* m_uploadPopup;
    float m_width;
    float m_height;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
};