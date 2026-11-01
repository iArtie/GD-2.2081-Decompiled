#pragma once

#include "cocos2d.h"

class MultiplayerLayer : public cocos2d::CCLayer {
public:
    static MultiplayerLayer* create();
    static cocos2d::CCScene* scene();

    void keyBackClicked() override;

    void onBack(cocos2d::CCObject* sender);
    void onBtn1(cocos2d::CCObject* sender);
    void onBtn2(cocos2d::CCObject* sender);

    virtual ~MultiplayerLayer();
};