#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "FLAlertLayerProtocol.h"
#include <string>

class TutorialPopup : public FLAlertLayer {
public:
    static TutorialPopup* create(std::string frame);
    bool init(std::string frame) override;
    void animateMenu();
    void closeTutorial(cocos2d::CCObject* sender);
    void keyBackClicked() override;
    void show() override;
    virtual ~TutorialPopup();

protected:
    bool m_callbackRegistered = false;
    cocos2d::SEL_MenuHandler m_callbackSelector;
    cocos2d::CCNode* m_targetNode = nullptr;
    std::string m_currentSprite;
};