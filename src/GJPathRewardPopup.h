#pragma once

#include "cocos2d.h"

class GJPathRewardPopup : public FLAlertLayer {
public:
    void closePopup();
    static GJPathRewardPopup* create(int path);
    bool init(int path);
    void keyBackClicked() override;
    void onClaim(cocos2d::CCObject* sender);
    virtual ~GJPathRewardPopup();

private:
    int m_pathNumber;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
};