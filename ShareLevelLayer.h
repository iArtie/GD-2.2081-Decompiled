#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"

class GJGameLevel;

class ShareLevelLayer : public FLAlertLayer {
public:
    static ShareLevelLayer* create(GJGameLevel* level);
    bool init(GJGameLevel* level) override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onSettings(cocos2d::CCObject* sender);
    void onShare(cocos2d::CCObject* sender);
    void selectRating(cocos2d::CCObject* sender);
    void setupStars();
    virtual ~ShareLevelLayer();

protected:
    int m_starsRequested = 0;
    cocos2d::CCArray* m_starButtons = nullptr;
    GJGameLevel* m_level = nullptr;
    cocos2d::CCSprite* m_difficultySprite = nullptr;
};