#pragma once

#include "Platform/cocos2d.h"

class GJUserScore;
class UploadActionPopup;
class CCMenuItemToggler;
class FLAlertLayer;
class TableViewCell;

class GJRequestCell : public TableViewCell {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    GJRequestCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromScore(GJUserScore* score);
    void markAsRead();
    void onClosePopup(UploadActionPopup* popup);
    void onDeleteRequest(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    void onViewFriendRequest(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    void updateToggle();
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);
    virtual ~GJRequestCell();

private:
    GJUserScore* m_score;
    UploadActionPopup* m_popup;
    CCMenuItemToggler* m_toggler;
    float m_width;
    float m_height;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
};