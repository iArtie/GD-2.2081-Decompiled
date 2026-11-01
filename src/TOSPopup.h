#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"

class TOSPopup : public FLAlertLayer {
public:
    static TOSPopup* create();
    bool init() override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onPrivacy(cocos2d::CCObject* sender);
    void onTOS(cocos2d::CCObject* sender);
    virtual ~TOSPopup();
};