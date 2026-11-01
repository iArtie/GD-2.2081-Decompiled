#pragma once

#include "Platform/cocos2d.h"

class CCBlockLayer : public cocos2d::CCLayerColor {
public:
    static CCBlockLayer* create();

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void draw() override;
    virtual void registerWithTouchDispatcher() override;
    virtual void keyBackClicked() override;

    bool init();
    void customSetup();
    void decrementForcePrio();
    void disableUI();
    void enableUI();
    void enterAnimFinished();
    void enterLayer();
    void exitLayer();
    void hideLayer(bool instant);
    void incrementForcePrio();
    void layerHidden();
    void layerVisible();
    void showLayer(bool instant);

    ~CCBlockLayer();

protected:
    bool m_closeOnHide;
    bool m_forcePrioRegistered;
};