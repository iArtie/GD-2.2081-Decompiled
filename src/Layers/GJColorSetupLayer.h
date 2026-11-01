// GJColorSetupLayer.h
#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Editor/ColorSelectDelegate.h"
#include "Editor/ColorSetupDelegate.h"

class LevelSettingsObject;
class ColorChannelSprite;
class CCMenuItemSpriteExtra;

class GJColorSetupLayer : public FLAlertLayer, public ColorSelectDelegate, public FLAlertLayerProtocol {
public:
    static GJColorSetupLayer* create(LevelSettingsObject* object);
    bool init(LevelSettingsObject* object);
    
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onColor(cocos2d::CCObject* sender);
    void onPage(cocos2d::CCObject* sender);
    void showPage(int page);
    void updateSpriteColors();
    
    virtual ~GJColorSetupLayer();

private:
    void updateSpriteColor(ColorChannelSprite* sprite, cocos2d::CCLabelBMFont* label, int colorID);

    ColorSetupDelegate* m_delegate;
    int m_colorID;
    LevelSettingsObject* m_settingsObject;
    int m_page;
    int m_colorsPerPage;
    int m_totalPages;
    bool m_closeOnSelect;
    bool m_noElasticity;
    
    cocos2d::CCDictionary* m_unk290;
    cocos2d::CCDictionary* m_unk298;
    cocos2d::CCArray* m_colorSprites;
    cocos2d::CCArray* m_colorLabels;
    cocos2d::CCMenu* m_buttonMenu;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    cocos2d::CCLayer* m_mainLayer;
};