#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCBlockLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"

class Slider;

class PauseLayer : public CCBlockLayer, public FLAlertLayerProtocol {
public:
    static PauseLayer* create(bool unfocused);

    bool init(bool unfocused);

    void customSetup() override;

    virtual void keyBackClicked() override;
    virtual void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    virtual void keyUp(cocos2d::enumKeyCodes key, double timestamp) override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void goEdit();
    void musicSliderChanged(cocos2d::CCObject* sender);
    void onEdit(cocos2d::CCObject* sender);
    void onHelp(cocos2d::CCObject* sender);
    void onNormalMode(cocos2d::CCObject* sender);
    void onPracticeMode(cocos2d::CCObject* sender);
    void onQuit(cocos2d::CCObject* sender);
    void onRecordReplays(cocos2d::CCObject* sender);
    void onReplay(cocos2d::CCObject* sender);
    void onRestart(cocos2d::CCObject* sender);
    void onRestartFull(cocos2d::CCObject* sender);
    void onResume(cocos2d::CCObject* sender);
    void onSettings(cocos2d::CCObject* sender);
    void onTime(cocos2d::CCObject* sender);
    void onTryEdit(cocos2d::CCObject* sender);
    void setupProgressBars();
    void sfxSliderChanged(cocos2d::CCObject* sender);
    void tryQuit(cocos2d::CCObject* sender);
    void tryShowBanner(float dt);

    virtual ~PauseLayer();

protected:
    bool m_unfocused = false;
    bool m_tryingQuit = false;
};