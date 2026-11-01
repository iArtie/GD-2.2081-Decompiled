#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "DemonFilterDelegate.h"

class DemonFilterSelectLayer : public FLAlertLayer {
public:
    static DemonFilterSelectLayer* create();
    bool init();
    
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void selectRating(cocos2d::CCObject* sender);
    
    void setDelegate(DemonFilterDelegate* delegate) { m_delegate = delegate; }
    int getCurrentDemon() const { return m_currentDemon; }
    
    virtual ~DemonFilterSelectLayer();

protected:
    DemonFilterDelegate* m_delegate;
    int m_currentDemon;
    cocos2d::CCArray* m_demons;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
};