#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include <string>

class FLAlertLayer;
class LevelSettingsObject;
class LevelEditorLayer;
class LevelSettingsDelegate;
class SongSelectNode;
class SelectArtLayer;
class SelectSettingLayer;
class ColorAction;
class ColorChannelSprite;
class CCTextInputNode;
class CCMenuItemSpriteExtra;
class CCMenuItemToggler;
class ButtonSprite;
class GJColorSetupLayer;
class LevelOptionsLayer;
class SelectFontLayer;
class ColorSelectPopup;
class GJEffectManager;
class EditorUI;

class LevelSettingsLayer : public FLAlertLayer,
                           public TextInputDelegate,
                           public ColorSelectDelegate,
                           public SelectArtDelegate,
                           public SelectSettingDelegate {
public:
    static LevelSettingsLayer* create(LevelSettingsObject* object, LevelEditorLayer* layer);

    bool init(LevelSettingsObject* object, LevelEditorLayer* layer);
    void keyBackClicked() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void onBGArt(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onCol(cocos2d::CCObject* sender);
    void onDisable(cocos2d::CCObject* sender);
    void onFGArt(cocos2d::CCObject* sender);
    void onGArt(cocos2d::CCObject* sender);
    void onGameplayMode(cocos2d::CCObject* sender);
    void onLiveEdit(cocos2d::CCObject* sender);
    void onMode(cocos2d::CCObject* sender);
    void onOptionToggle(cocos2d::CCObject* sender);
    void onSelectFont(cocos2d::CCObject* sender);
    void onSelectMode(cocos2d::CCObject* sender);
    void onSelectSpeed(cocos2d::CCObject* sender);
    void onSettings(cocos2d::CCObject* sender);
    void onShowPicker(cocos2d::CCObject* sender);
    void onSpeed(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void selectArtClosed(SelectArtLayer* layer);
    void selectSettingClosed(SelectSettingLayer* layer);
    void showPicker(ColorAction* action);
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void updateColorSprite(ColorChannelSprite* sprite);
    void updateColorSprites();
    void updateGameplayModeButtons();

    virtual ~LevelSettingsLayer();

protected:
    LevelSettingsObject* m_settingsObject = nullptr;
    CCTextInputNode* m_orderInput = nullptr;
    CCTextInputNode* m_channelInput = nullptr;
    SongSelectNode* m_songSelectNode = nullptr;
    Speed m_speed = Speed::Normal;
    LevelSettingsDelegate* m_delegate = nullptr;
    LevelEditorLayer* m_editorLayer = nullptr;
    cocos2d::CCSprite* m_groundSprite = nullptr;
    cocos2d::CCSprite* m_middlegroundSprite = nullptr;
    cocos2d::CCSprite* m_backgroundSprite = nullptr;
    bool m_disableInput = false;
    CCMenuItemSpriteExtra* m_classicButton = nullptr;
    CCMenuItemSpriteExtra* m_platformerButton = nullptr;
    cocos2d::CCArray* m_modeToggles = nullptr;
    cocos2d::CCArray* m_speedButtons = nullptr;
    ColorChannelSprite* m_bgColorSprite = nullptr;
    ColorChannelSprite* m_gColorSprite = nullptr;
    ColorChannelSprite* m_g2ColorSprite = nullptr;
    ColorChannelSprite* m_lineColorSprite = nullptr;
    ColorChannelSprite* m_mgColorSprite = nullptr;
    ColorChannelSprite* m_mg2ColorSprite = nullptr;
    cocos2d::CCSprite* m_moreColorsSprite = nullptr;
    cocos2d::CCSprite* m_speedSprite = nullptr;
    cocos2d::CCSprite* m_modeSprite = nullptr;
};