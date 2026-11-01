#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "NumberInputDelegate.h"

class GJGameLevel;
class CCMenuItemToggler;

class ShareLevelSettingsLayer : public FLAlertLayer, public NumberInputDelegate {
public:
    static ShareLevelSettingsLayer* create(GJGameLevel* level);
    bool init(GJGameLevel* level) override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onUnlisted(cocos2d::CCObject* sender);
    void onUnlistedFriendsOnly(cocos2d::CCObject* sender);
    void updateSettingsState();
    virtual ~ShareLevelSettingsLayer();

protected:
    GJGameLevel* m_level = nullptr;
    CCMenuItemToggler* m_friendsOnlyToggler = nullptr;
    cocos2d::CCLabelBMFont* m_friendsOnlyLabel = nullptr;
    cocos2d::CCLabelBMFont* m_passwordLabel = nullptr;
};