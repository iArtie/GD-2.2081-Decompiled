#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "CustomSongDelegate.h"
#include "CustomSongLayerDelegate.h"
#include "FLAlertLayerProtocol.h"
#include <string>

class LevelSettingsObject;
class CustomSongWidget;
class CCMenuItemSpriteExtra;
class CCLabelBMFont;

class SongSelectNode : public FLAlertLayerProtocol, public CustomSongDelegate, public CustomSongLayerDelegate {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void audioNext(cocos2d::CCObject* sender);
    void audioPrevious(cocos2d::CCObject* sender);
    void customSongLayerClosed() override;
    int getActiveSongID();
    LevelSettingsObject* getLevelSettings();
    std::string getSongFileName() override;
    void onOpenCustomSong(cocos2d::CCObject* sender);
    void onSongMode(cocos2d::CCObject* sender);
    void selectSong(int id);
    void showCustomSongSelect();
    void songIDChanged(int id) override;
    void updateAudioLabel();
    void updateWidgetVisibility();
    virtual ~SongSelectNode();

protected:
    int m_selectedSongID = 0;
    bool m_songChanged = false;
    LevelSettingsObject* m_settingsObject = nullptr;
    cocos2d::CCLabelBMFont* m_audioLabel = nullptr;
    bool m_isCustomSong = false;
    CustomSongWidget* m_songWidget = nullptr;
    CCMenuItemSpriteExtra* m_selectButton = nullptr;
    CCMenuItemSpriteExtra* m_changeButton = nullptr;
    CCMenuItemSpriteExtra* m_customButton = nullptr;
};