#pragma once

#include "Platform/cocos2d.h"

class GJUserMessage;
class UploadActionPopup;
class CCMenuItemToggler;
class FLAlertLayer;
class TableViewCell;

class GJMessageCell : public TableViewCell {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    GJMessageCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromMessage(GJUserMessage* message);
    void markAsRead();
    void onClosePopup(UploadActionPopup* popup);
    void onDeleteMessage(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    void onViewMessage(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    void updateToggle();
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);
    virtual ~GJMessageCell();

private:
    GJUserMessage* m_message;
    UploadActionPopup* m_popup;
    CCMenuItemToggler* m_toggler;
    float m_width;
    float m_height;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
};