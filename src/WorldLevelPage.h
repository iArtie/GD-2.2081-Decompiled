#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "FLAlertLayer.h"

class GJGameLevel;
class GJWorldNode;
class CCMenuItemSpriteExtra;

class WorldLevelPage : public FLAlertLayer {
public:
    static WorldLevelPage* create(GJGameLevel* level, GJWorldNode* node);
    bool init(GJGameLevel* level, GJWorldNode* node);

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onPlay(cocos2d::CCObject* sender);
    void onSong(cocos2d::CCObject* sender);
    void show();

    ~WorldLevelPage();

protected:
    GJGameLevel* m_level = nullptr;
    GJWorldNode* m_worldNode = nullptr;
};