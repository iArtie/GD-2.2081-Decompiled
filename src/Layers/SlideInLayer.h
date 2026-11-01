#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

class SlideInLayerDelegate;

class SlideInLayer : public cocos2d::CCLayerColor {
public:
    static SlideInLayer* create();
    bool init() override;
    void keyBackClicked() override;
    void registerWithTouchDispatcher() override;
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void customSetup();
    void disableUI();
    void draw() override;
    void enableUI();
    virtual void enterAnimFinished();
    void enterLayer();
    void exitLayer(cocos2d::CCObject* sender);
    void hideLayer(bool instant);
    virtual void layerHidden();
    virtual void layerVisible();
    void showLayer(bool instant);
    virtual ~SlideInLayer();

protected:
    cocos2d::CCMenu* m_buttonMenu = nullptr;
    SlideInLayerDelegate* m_delegate = nullptr;
    bool m_closeOnHide = true;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    cocos2d::CCPoint m_endPosition;
    cocos2d::CCPoint m_startPosition;
    cocos2d::CCSprite* m_exitSprite = nullptr;
    CCMenuItemSpriteExtra* m_exitButton = nullptr;
};