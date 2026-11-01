#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCBlockLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"

class LevelEditorLayer;

class EditorPauseLayer : public CCBlockLayer, public FLAlertLayerProtocol {
public:
    static EditorPauseLayer* create(LevelEditorLayer* layer);
    bool init(LevelEditorLayer* layer);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    
    void customSetup();
    void doResetUnused();
    void onAlignX(cocos2d::CCObject* sender);
    void onAlignY(cocos2d::CCObject* sender);
    void onBuildHelper(cocos2d::CCObject* sender);
    void onCopyWColor(cocos2d::CCObject* sender);
    void onCreateExtras(cocos2d::CCObject* sender);
    void onCreateLoop(cocos2d::CCObject* sender);
    void onCreateTemplate(cocos2d::CCObject* sender);
    void onExitEditor(cocos2d::CCObject* sender);
    void onExitNoSave(cocos2d::CCObject* sender);
    void onHelp(cocos2d::CCObject* sender);
    void onKeybindings(cocos2d::CCObject* sender);
    void onNewGroupX(cocos2d::CCObject* sender);
    void onNewGroupY(cocos2d::CCObject* sender);
    void onOptions(cocos2d::CCObject* sender);
    void onPasteWColor(cocos2d::CCObject* sender);
    void onReGroup(cocos2d::CCObject* sender);
    void onResetUnusedColors(cocos2d::CCObject* sender);
    void onResume(cocos2d::CCObject* sender);
    void onSave(cocos2d::CCObject* sender);
    void onSaveAndExit(cocos2d::CCObject* sender);
    void onSaveAndPlay(cocos2d::CCObject* sender);
    void onSelectAll(cocos2d::CCObject* sender);
    void onSelectAllLeft(cocos2d::CCObject* sender);
    void onSelectAllRight(cocos2d::CCObject* sender);
    void onSong(cocos2d::CCObject* sender);
    void onUnlockAllLayers(cocos2d::CCObject* sender);
    void playStep2();
    void playStep3();
    void saveLevel();
    void toggleDebugDraw(cocos2d::CCObject* sender);
    void toggleEditorBackground(cocos2d::CCObject* sender);
    void toggleEditorColorMode(cocos2d::CCObject* sender);
    void toggleEditorGrid(cocos2d::CCObject* sender);
    void toggleEditorGround(cocos2d::CCObject* sender);
    void toggleEffectDuration(cocos2d::CCObject* sender);
    void toggleEffectLines(cocos2d::CCObject* sender);
    void toggleFollowPlayer(cocos2d::CCObject* sender);
    void toggleGridOnTop(cocos2d::CCObject* sender);
    void toggleHideInvisible(cocos2d::CCObject* sender);
    void toggleIgnoreDamage(cocos2d::CCObject* sender);
    void togglePlaytestMusic(cocos2d::CCObject* sender);
    void togglePreviewAnim(cocos2d::CCObject* sender);
    void togglePreviewParticles(cocos2d::CCObject* sender);
    void togglePreviewShaders(cocos2d::CCObject* sender);
    void toggleRecordOrder(cocos2d::CCObject* sender);
    void toggleSelectFilter(cocos2d::CCObject* sender);
    void toggleShowObjectInfo(cocos2d::CCObject* sender);
    void uncheckAllPortals(cocos2d::CCObject* sender);
    void updateSongButton();
    
    virtual ~EditorPauseLayer();

protected:
    LevelEditorLayer* m_editorLayer;
    CCMenuItemSpriteExtra* m_guidelinesOnButton;
    CCMenuItemSpriteExtra* m_guidelinesOffButton;
    bool m_saved;
};