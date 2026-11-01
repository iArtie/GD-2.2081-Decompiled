#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "TextInputDelegate.h"

class CCTextInputNode;
class CCMenuItemToggler;
class InfoAlertButton;

class GJAccountSettingsLayer : public FLAlertLayer, public TextInputDelegate {
public:
    static GJAccountSettingsLayer* create(int accountID);
    bool init(int accountID);
    void keyBackClicked();
    void onClose(cocos2d::CCObject* sender);
    void onMessageSetting(cocos2d::CCObject* sender);
    void onFriendRequests(cocos2d::CCObject* sender);
    void onCommentSetting(cocos2d::CCObject* sender);
    void onUpdate(cocos2d::CCObject* sender);
    void updateScoreValues();
    void textInputReturn(CCTextInputNode* node) override;
    void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    void closeAccInputNodes(cocos2d::CCArray* nodes);
    virtual ~GJAccountSettingsLayer();

private:
    CCMenuItemToggler* createToggleButton(const std::string& text, cocos2d::SEL_MenuHandler handler, bool enabled, cocos2d::CCMenu* menu, cocos2d::CCPoint pos, float width, float scale);

    int m_accountID;
    int m_messageStatus;
    int m_friendStatus;
    int m_commentHistoryStatus;
    bool m_noElasticity;
    int m_joystickConnected;

    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCArray* m_messageSettings;
    cocos2d::CCArray* m_friendRequestSettings;
    cocos2d::CCArray* m_commentSettings;
    cocos2d::CCArray* m_inputs;

    // Links map for storing social media URLs
    std::map<int, std::string> m_links;
};