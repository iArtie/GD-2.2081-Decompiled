#pragma once

#include "Platform/cocos2d.h"

class GJDropDownLayerDelegate;

class GJDropDownLayer : public cocos2d::CCLayerColor
{
public:
    static GJDropDownLayer* create(const char* title);
    
    bool init(const char* title);
    void customSetup();
    void draw();
    
    void enterLayer();
    void exitLayer(cocos2d::CCObject* sender);
    void hideLayer(bool instant);
    void showLayer(bool instant);
    void layerHidden();
    void layerVisible();
    void enterAnimFinished();
    
    void enableUI();
    void disableUI();
    
    void keyBackClicked();
    void registerWithTouchDispatcher();
    
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    
    virtual ~GJDropDownLayer();

protected:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    GJDropDownLayerDelegate* m_delegate;
    cocos2d::CCPoint m_startPosition;
    cocos2d::CCPoint m_endPosition;
    bool m_fastMenu;
    bool m_closeOnHide;
};