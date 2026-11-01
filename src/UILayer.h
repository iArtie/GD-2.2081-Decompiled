#pragma once

#include "cocos2d.h"
#include "cocos2d.h"
#include "GJBaseGameLayer.h"

class CCMenuItemSpriteExtra;
class GJUINode;

class UILayer : public cocos2d::CCLayerColor {
public:
    static UILayer* create(GJBaseGameLayer* layer);
    bool init(GJBaseGameLayer* layer) override;
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void registerWithTouchDispatcher() override;
    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    void keyUp(cocos2d::enumKeyCodes key, double timestamp) override;
    void draw() override;
    void disableMenu();
    void doPause();
    void editorPlaytest(bool visible);
    void enableEditorMode();
    void enableMenu();
    void handleKeypress(cocos2d::enumKeyCodes key, bool down, double timestamp);
    bool isJumpButtonPressed();
    void onCheck(cocos2d::CCObject* sender);
    void onDeleteCheck(cocos2d::CCObject* sender);
    void onPause(cocos2d::CCObject* sender);
    void refreshDpad();
    void resetAllButtons();
    void resetUINodeState();
    void toggleCheckpointsMenu(bool visible);
    void toggleMenuVisibility(bool visible);
    void togglePlatformerMode(bool platformer);
    void updateCheckState();
    void updateDualMode(bool dual);
    void updateUINodeVisibility(bool visible);
    virtual ~UILayer();

protected:
    cocos2d::CCMenu* m_checkpointMenu = nullptr;
    cocos2d::CCArray* m_controllerButtons = nullptr;
    cocos2d::CCArray* m_uiNodes = nullptr;
    bool m_dualMode = false;
    CCMenuItemSpriteExtra* m_pauseBtn = nullptr;
    int m_p1TouchId = -1;
    int m_p2TouchId = -1;
    bool m_editorMode = false;
    bool m_checkpointBtnDown = false;
    GJBaseGameLayer* m_gameLayer = nullptr;
    bool m_p2Jumping = false;
    bool m_p1Jumping = false;
    bool m_inPlatformer = false;
    bool m_initialized = false;
    bool m_dpadType = false;
};