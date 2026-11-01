#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "TextInputDelegate.h"
#include "GJDropDownLayerDelegate.h"
#include "MusicBrowserDelegate.h"
#include "CustomSongDelegate.h"
#include "CustomSongLayerDelegate.h"

class CustomSongLayer : public FLAlertLayer, public TextInputDelegate, public GJDropDownLayerDelegate, public MusicBrowserDelegate {
public:
    static CustomSongLayer* create(CustomSongDelegate* delegate);
    bool init(CustomSongDelegate* delegate);
    
    void dropDownLayerWillClose(GJDropDownLayer* layer) override;
    void musicBrowserClosed(MusicBrowser* browser) override;
    
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onCreateLines(cocos2d::CCObject* sender);
    void onMusicBrowser(cocos2d::CCObject* sender);
    void onNCSBrowser(cocos2d::CCObject* sender);
    void onNewgroundsBrowser(cocos2d::CCObject* sender);
    void onOptions(cocos2d::CCObject* sender);
    void onSearch(cocos2d::CCObject* sender);
    void onSongBrowser(cocos2d::CCObject* sender);
    
    void show();
    void showNewgroundsMessage();
    
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    void textInputReturn(CCTextInputNode* node) override;
    void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    
    virtual ~CustomSongLayer();

protected:
    CustomSongLayerDelegate* m_delegate;
    CustomSongDelegate* m_songDelegate;
    CustomSongWidget* m_songWidget;
    CCTextInputNode* m_songIDInput;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    int m_joystickConnected;
};