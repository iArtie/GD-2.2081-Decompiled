#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include <string>

class StarInfoPopup : public FLAlertLayer {
public:
    static StarInfoPopup* createFromString(std::string str);
    static StarInfoPopup* createFromStringMoons(std::string str);
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    virtual ~StarInfoPopup();
};