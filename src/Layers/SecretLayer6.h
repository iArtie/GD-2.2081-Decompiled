#pragma once

#include "Platform/cocos2d.h"

class SecretGame01Layer;

class SecretLayer6 : public cocos2d::CCLayer {
public:
    static SecretLayer6* create();
    static cocos2d::CCScene* scene();
    bool init() override;
    void keyBackClicked() override;
    void onBack(cocos2d::CCObject* sender);
    void startGame01();

    ~SecretLayer6();

protected:
    SecretGame01Layer* m_gameLayer = nullptr;
};