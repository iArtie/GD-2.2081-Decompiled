#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"

class GJBigSprite;
class GJMapObject;
class LevelEditorLayer;
class SimplePlayer;
class CCMenuItemSpriteExtra;

class MapSelectLayer : public cocos2d::CCLayer {
public:
    static MapSelectLayer* create();
    static cocos2d::CCScene* scene();

    bool init() override;
    void keyBackClicked() override;
    void onExit() override;
    void registerWithTouchDispatcher() override;
    void update(float dt) override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    bool checkTouchMonster(cocos2d::CCPoint position);
    GJMapObject* createObjectAtPoint(cocos2d::CCPoint position);
    cocos2d::CCPoint getConstrainedMapPos(cocos2d::CCPoint position);
    void onBack(cocos2d::CCObject* sender);
    void scrollWheel(float y, float x);

    virtual ~MapSelectLayer();

protected:
    GJBigSprite* m_bigSprite = nullptr;
    bool m_backPressed = false;
    bool m_blockExit = false;
    bool m_unloadTexturesOnExit = false;
    bool m_inTouch = false;
    double m_lastTouchTime = 0.0;
    cocos2d::CCPoint m_mapPos;
    cocos2d::CCArray* m_monsters = nullptr;
    cocos2d::CCNode* m_mainLayer = nullptr;
    cocos2d::CCPoint m_lastTouchPos;
    bool m_editMode = false;
    LevelEditorLayer* m_editorLayer = nullptr;
    SimplePlayer* m_player = nullptr;
};