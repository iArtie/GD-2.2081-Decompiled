#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/CharacterColorDelegate.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Sprites/SimplePlayer.h"

class CharacterColorPage : public FLAlertLayer, public FLAlertLayerProtocol {
public:
    static CharacterColorPage* create();

    virtual void show() override;
    virtual bool init() override;
    virtual void keyBackClicked() override;
    virtual void registerWithTouchDispatcher() override;
    virtual void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;

    int activeColorForMode(int mode);
    bool checkColor(int id, UnlockType type);
    int colorForIndex(int index);
    void createColorMenu();
    cocos2d::CCPoint offsetForIndex(int index);
    void onClose(cocos2d::CCObject* sender);
    void onMode(cocos2d::CCObject* sender);
    void onPlayerColor(cocos2d::CCObject* sender);
    void toggleGlow(cocos2d::CCObject* sender);
    void toggleGlowItems(bool visible);
    void toggleShip(cocos2d::CCObject* sender);
    void updateColorMode(int mode);
    void updateIconColors();

    void setDelegate(CharacterColorDelegate* delegate) { m_delegate = delegate; }

    ~CharacterColorPage();

private:
    CharacterColorDelegate* m_delegate;
    bool m_noElasticity;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    CCMenuItemToggler* m_glowToggler;
    cocos2d::CCLabelBMFont* m_glowLabel;
    int m_colorMode;
    cocos2d::CCDictionary* m_colorButtons;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCArray* m_cursors;
    cocos2d::CCArray* m_modeButtons;
    cocos2d::CCArray* m_playerObjects;
    float m_width;
    float m_height;
};