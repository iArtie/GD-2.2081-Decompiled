#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"

class RateLevelDelegate;
class CCMenuItemSpriteExtra;

class RateLevelLayer : public FLAlertLayer {
public:
    static RateLevelLayer* create(int levelID);

    bool init(int levelID);
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onRate(cocos2d::CCObject* sender);
    void selectRating(cocos2d::CCObject* sender);

    virtual ~RateLevelLayer();

protected:
    RateLevelDelegate* m_delegate = nullptr;
    int m_levelRate = -1;
    cocos2d::CCArray* m_difficulties = nullptr;
    CCMenuItemSpriteExtra* m_submitButton = nullptr;
    int m_levelID = 0;
};