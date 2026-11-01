#pragma once

#include "Platform/cocos2d.h"

class FLAlertLayer;

class NewgroundsInfoLayer : public FLAlertLayer {
public:
    static NewgroundsInfoLayer* create();

    bool init() override;

    virtual void keyBackClicked() override;
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void onArtists(cocos2d::CCObject* sender);
    void onChanges(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onGuidelines(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onNewgrounds(cocos2d::CCObject* sender);
    void onSupporter(cocos2d::CCObject* sender);

    virtual ~NewgroundsInfoLayer();
};