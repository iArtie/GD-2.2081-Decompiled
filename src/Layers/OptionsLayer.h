#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJDropDownLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"

class Slider;

class OptionsLayer : public GJDropDownLayer, public FLAlertLayerProtocol {
public:
    static OptionsLayer* create();

    void customSetup() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void exitLayer();
    void layerHidden();
    void musicSliderChanged(cocos2d::CCObject* sender);
    void onAccount(cocos2d::CCObject* sender);
    void onHelp(cocos2d::CCObject* sender);
    void onMenuMusic(cocos2d::CCObject* sender);
    void onOptions(cocos2d::CCObject* sender);
    void onProgressBar(cocos2d::CCObject* sender);
    void onRate(cocos2d::CCObject* sender);
    void onRecordReplays(cocos2d::CCObject* sender);
    void onSecretVault(cocos2d::CCObject* sender);
    void onSoundtracks(cocos2d::CCObject* sender);
    void onSupport(cocos2d::CCObject* sender);
    void onVideo(cocos2d::CCObject* sender);
    void sfxSliderChanged(cocos2d::CCObject* sender);
    void tryEnableRecord();

    virtual ~OptionsLayer();

protected:
    int m_layerChoice = 0;
    bool m_recordReplays = false;
    int m_lastVaultDialog = -1;
    cocos2d::CCMenu* m_optionsMenu = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    Slider* m_musicSlider = nullptr;
    Slider* m_sfxSlider = nullptr;
};