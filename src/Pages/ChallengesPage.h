#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/GJChallengeDelegate.h"
#include "Delegates/CurrencyRewardDelegate.h"

class CurrencyRewardLayer;
class LoadingCircle;
class GJChallengeItem;
class ChallengeNode;
class GJChallengeDelegate;

class ChallengesPage : public FLAlertLayer, public FLAlertLayerProtocol, public GJChallengeDelegate, public CurrencyRewardDelegate {
public:
    static ChallengesPage* create();

    virtual void show() override;
    virtual bool init() override;
    virtual void keyBackClicked() override;
    virtual void registerWithTouchDispatcher() override;
    virtual void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    virtual void currencyWillExit(CurrencyRewardLayer* layer) override;

    void challengeStatusFailed();
    void challengeStatusFinished();
    void exitNodeAtSlot(int position, float delay);
    void onClose(cocos2d::CCObject* sender);
    void tryGetChallenges();
    void updateDots();
    void updateTimers(float dt);

    ~ChallengesPage();

private:
    CurrencyRewardLayer* m_currencyRewardLayer;
    LoadingCircle* m_circle;
    bool m_triedToLoad;
    bool m_noElasticity;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    cocos2d::CCArray* m_dots;
    cocos2d::CCDictionary* m_challengeNodes;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLabelBMFont* m_countdownLabel;
    bool m_unkBool;
};