#pragma once

#include "cocos2d.h"

class WorldSelectLayer;
class GJGameLevel;
class CCMenuItemSpriteExtra;

class GJWorldNode : public cocos2d::CCNode {
public:
    float addDotsToLevel(int levelID, bool animate);
    static GJWorldNode* create(int type, WorldSelectLayer* layer);
    cocos2d::CCPoint dotPositionForLevel(int levelID, int index);
    bool init(int type, WorldSelectLayer* layer) override;
    void onLevel(cocos2d::CCObject* sender);
    void playStep1();
    void playStep2();
    void playStep3();
    cocos2d::CCPoint positionForLevelButton(int levelID);
    void unlockActiveItem();
    virtual ~GJWorldNode();

private:
    GJGameLevel* m_level;
    CCMenuItemSpriteExtra* m_activeButton;
    cocos2d::CCArray* m_activeObjects;
    cocos2d::CCSprite* m_arrowSprite;
    cocos2d::CCPoint m_levelPosition;
    cocos2d::ccColor3B m_particleColor;
    WorldSelectLayer* m_selectLayer;
    int m_islandType;
    bool m_islandUnlocked;
    bool m_unlocked;
};