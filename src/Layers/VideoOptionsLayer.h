#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"

class CCMenuItemSpriteExtra;
class CCMenuItemToggler;

class VideoOptionsLayer : public FLAlertLayer {
public:
    static VideoOptionsLayer* create();
    bool init() override;

    void keyBackClicked() override;
    void onAdvanced(cocos2d::CCObject* sender);
    void onApply(cocos2d::CCObject* sender);
    void onBorderless(cocos2d::CCObject* sender);
    void onBorderlessFix(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onFullscreen(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onResolutionNext(cocos2d::CCObject* sender);
    void onResolutionPrev(cocos2d::CCObject* sender);
    void onTextureQualityNext(cocos2d::CCObject* sender);
    void onTextureQualityPrev(cocos2d::CCObject* sender);
    void reloadMenu();
    void toggleResolution();
    void updateResolution(int resolution);
    void updateTextureQuality(int quality);

    ~VideoOptionsLayer();

protected:
    bool m_fullscreen = false;
    bool m_borderless = false;
    bool m_fix = false;
    cocos2d::CCArray* m_availableResolutions = nullptr;
    int m_currentResolution = 0;
    int m_textureQuality = 0;
    cocos2d::CCLabelBMFont* m_qualityLabel = nullptr;
    cocos2d::CCLabelBMFont* m_selectedResolutionLabel = nullptr;
    cocos2d::CCLabelBMFont* m_windowedResolutionLabel = nullptr;
    CCMenuItemSpriteExtra* m_prevResolutionBtn = nullptr;
    CCMenuItemSpriteExtra* m_nextResolutionBtn = nullptr;
    CCMenuItemToggler* m_borderlessToggle = nullptr;
    cocos2d::CCLabelBMFont* m_borderlessLabel = nullptr;
    CCMenuItemToggler* m_fixToggle = nullptr;
    cocos2d::CCLabelBMFont* m_fixLabel = nullptr;
};