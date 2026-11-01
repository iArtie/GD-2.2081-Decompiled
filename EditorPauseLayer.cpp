#include "EditorPauseLayer.h"
#include "LevelEditorLayer.h"
#include "EditorUI.h"
#include "GameManager.h"
#include "LocalLevelManager.h"
#include "FMODAudioEngine.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "GameToolbox.h"
#include "FLAlertLayer.h"
#include "EditorOptionsLayer.h"
#include "KeybindingsLayer.h"
#include "GJGameLoadingLayer.h"
#include "DrawGridLayer.h"
#include "GJGameLevel.h"
#include "LevelSettingsObject.h"
#include <cstdlib>

EditorPauseLayer* EditorPauseLayer::create(LevelEditorLayer* layer)
{
    EditorPauseLayer* pRet = new EditorPauseLayer();
    if (pRet && pRet->init(layer))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool EditorPauseLayer::init(LevelEditorLayer* layer)
{
    if (!CCBlockLayer::init())
        return false;
    
    m_editorLayer = layer;
    m_saved = false;
    m_guidelinesOffButton = nullptr;
    m_guidelinesOnButton = nullptr;
    
    return true;
}

void EditorPauseLayer::customSetup()
{
    setOpacity(255);
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Main menu buttons
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    addChild(menu);
    
    // Resume button
    ButtonSprite* resumeBtn = ButtonSprite::create("Resume", 0, 1, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 28.0f);
    CCMenuItemSpriteExtra* resumeItem = CCMenuItemSpriteExtra::create(
        resumeBtn, nullptr, this, menu_selector(EditorPauseLayer::onResume));
    menu->addChild(resumeItem);
    
    // Save and Play button
    ButtonSprite* savePlayBtn = ButtonSprite::create("Save and Play", 0, 1, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 28.0f);
    CCMenuItemSpriteExtra* savePlayItem = CCMenuItemSpriteExtra::create(
        savePlayBtn, nullptr, this, menu_selector(EditorPauseLayer::onSaveAndPlay));
    menu->addChild(savePlayItem);
    
    // Save and Exit button
    ButtonSprite* saveExitBtn = ButtonSprite::create("Save and Exit", 0, 1, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 28.0f);
    CCMenuItemSpriteExtra* saveExitItem = CCMenuItemSpriteExtra::create(
        saveExitBtn, nullptr, this, menu_selector(EditorPauseLayer::onSaveAndExit));
    menu->addChild(saveExitItem);
    
    // Save button
    ButtonSprite* saveBtn = ButtonSprite::create("Save", 0, 1, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 28.0f);
    CCMenuItemSpriteExtra* saveItem = CCMenuItemSpriteExtra::create(
        saveBtn, nullptr, this, menu_selector(EditorPauseLayer::onSave));
    menu->addChild(saveItem);
    
    // Exit button
    ButtonSprite* exitBtn = ButtonSprite::create("Exit", 0, 1, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 28.0f);
    CCMenuItemSpriteExtra* exitItem = CCMenuItemSpriteExtra::create(
        exitBtn, nullptr, this, menu_selector(EditorPauseLayer::onExitNoSave));
    menu->addChild(exitItem);
    
    menu->alignItemsVerticallyWithPadding(14.0f);
    
    // If read-only level, disable save buttons
    if (m_editorLayer->m_level->m_levelType != Editor)
    {
        savePlayItem->setEnabled(false);
        saveExitItem->setEnabled(false);
        saveItem->setEnabled(false);
    }
    
    // Bottom menu with various tools
    cocos2d::CCMenu* bottomMenu = cocos2d::CCMenu::create();
    addChild(bottomMenu);
    
    // Song toggle buttons
    cocos2d::CCSprite* audioOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_audioOnBtn_001.png");
    m_guidelinesOffButton = CCMenuItemSpriteExtra::create(
        audioOn, nullptr, this, menu_selector(EditorPauseLayer::onSong));
    
    cocos2d::CCSprite* audioOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_audioOffBtn_001.png");
    m_guidelinesOnButton = CCMenuItemSpriteExtra::create(
        audioOff, nullptr, this, menu_selector(EditorPauseLayer::onSong));
    
    bottomMenu->addChild(m_guidelinesOffButton);
    bottomMenu->addChild(m_guidelinesOnButton);
    
    // Help button
    cocos2d::CCSprite* helpSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_helpBtn_001.png");
    CCMenuItemSpriteExtra* helpItem = CCMenuItemSpriteExtra::create(
        helpSprite, nullptr, this, menu_selector(EditorPauseLayer::onHelp));
    bottomMenu->addChild(helpItem);
    
    bottomMenu->alignItemsHorizontally();
    bottomMenu->setPosition(cocos2d::CCPoint(centerX, cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 28.0f));
    
    // Position on/off buttons together
    m_guidelinesOnButton->setPosition(m_guidelinesOffButton->getPosition());
    updateSongButton();
    
    // Create all tool buttons
    float yOffset = 0.0f;
    float xPos = cocos2d::CCDirector::sharedDirector()->getScreenRight() - 50.0f;
    
    // Uncheck Portals
    ButtonSprite* uncheckPortals = ButtonSprite::create("Uncheck\nPortals", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    uncheckPortals->setScale(0.8f);
    CCMenuItemSpriteExtra* uncheckBtn = CCMenuItemSpriteExtra::create(
        uncheckPortals, nullptr, this, menu_selector(EditorPauseLayer::uncheckAllPortals));
    bottomMenu->addChild(uncheckBtn);
    uncheckBtn->setPosition(bottomMenu->convertToNodeSpace(cocos2d::CCPoint(xPos, cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 30.0f)));
    yOffset += 30.0f;
    
    // Reset Unused Colors
    ButtonSprite* resetUnused = ButtonSprite::create("Reset\nUnused", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    resetUnused->setScale(0.8f);
    CCMenuItemSpriteExtra* resetBtn = CCMenuItemSpriteExtra::create(
        resetUnused, nullptr, this, menu_selector(EditorPauseLayer::onResetUnusedColors));
    bottomMenu->addChild(resetBtn);
    resetBtn->setPosition(uncheckBtn->getPosition() + cocos2d::CCPoint(0, yOffset));
    yOffset += 30.0f;
    
    // Unlock Layers
    ButtonSprite* unlockLayers = ButtonSprite::create("Unlock\nLayers", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    unlockLayers->setScale(0.8f);
    CCMenuItemSpriteExtra* unlockBtn = CCMenuItemSpriteExtra::create(
        unlockLayers, nullptr, this, menu_selector(EditorPauseLayer::onUnlockAllLayers));
    bottomMenu->addChild(unlockBtn);
    unlockBtn->setPosition(resetBtn->getPosition() + cocos2d::CCPoint(0, 30.0f));
    yOffset += 30.0f;
    
    // Create Extras
    ButtonSprite* createExtras = ButtonSprite::create("Create\nExtras", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    createExtras->setScale(0.8f);
    CCMenuItemSpriteExtra* extrasBtn = CCMenuItemSpriteExtra::create(
        createExtras, nullptr, this, menu_selector(EditorPauseLayer::onCreateExtras));
    bottomMenu->addChild(extrasBtn);
    extrasBtn->setPosition(unlockBtn->getPosition() + cocos2d::CCPoint(0, 30.0f));
    yOffset += 30.0f;
    
    // Paste w/Color
    ButtonSprite* pasteColor = ButtonSprite::create("Paste+\nColor", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    pasteColor->setScale(0.8f);
    CCMenuItemSpriteExtra* pasteBtn = CCMenuItemSpriteExtra::create(
        pasteColor, nullptr, this, menu_selector(EditorPauseLayer::onPasteWColor));
    bottomMenu->addChild(pasteBtn);
    pasteBtn->setPosition(extrasBtn->getPosition() + cocos2d::CCPoint(0, 30.0f));
    yOffset += 30.0f;
    
    // Copy w/Color
    ButtonSprite* copyColor = ButtonSprite::create("Copy+\nColor", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    copyColor->setScale(0.8f);
    CCMenuItemSpriteExtra* copyBtn = CCMenuItemSpriteExtra::create(
        copyColor, nullptr, this, menu_selector(EditorPauseLayer::onCopyWColor));
    bottomMenu->addChild(copyBtn);
    copyBtn->setPosition(pasteBtn->getPosition() + cocos2d::CCPoint(0, 30.0f));
    yOffset += 30.0f;
    
    // Build Helper
    ButtonSprite* buildHelper = ButtonSprite::create("Build\nHelper", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    buildHelper->setScale(0.8f);
    CCMenuItemSpriteExtra* buildBtn = CCMenuItemSpriteExtra::create(
        buildHelper, nullptr, this, menu_selector(EditorPauseLayer::onBuildHelper));
    bottomMenu->addChild(buildBtn);
    cocos2d::CCPoint buildPos = buildBtn->getParent()->convertToNodeSpace(cocos2d::CCPoint(centerX - 50.0f, 30.0f));
    buildBtn->setPosition(buildPos);
    
    // Create Loop
    ButtonSprite* createLoop = ButtonSprite::create("Create\nLoop", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    createLoop->setScale(0.8f);
    CCMenuItemSpriteExtra* loopBtn = CCMenuItemSpriteExtra::create(
        createLoop, nullptr, this, menu_selector(EditorPauseLayer::onCreateLoop));
    bottomMenu->addChild(loopBtn);
    loopBtn->setPosition(buildBtn->getPosition() + cocos2d::CCPoint(-50.0f, 30.0f));
    
    // Re-Group
    ButtonSprite* regroup = ButtonSprite::create("Re-\nGroup", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    regroup->setScale(0.8f);
    CCMenuItemSpriteExtra* regroupBtn = CCMenuItemSpriteExtra::create(
        regroup, nullptr, this, menu_selector(EditorPauseLayer::onReGroup));
    bottomMenu->addChild(regroupBtn);
    regroupBtn->setPosition(loopBtn->getPosition() + cocos2d::CCPoint(0, 30.0f));
    
    // Align X
    ButtonSprite* alignX = ButtonSprite::create("AlignX", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    alignX->setScale(0.8f);
    CCMenuItemSpriteExtra* alignXBtn = CCMenuItemSpriteExtra::create(
        alignX, nullptr, this, menu_selector(EditorPauseLayer::onAlignX));
    bottomMenu->addChild(alignXBtn);
    alignXBtn->setPosition(buildBtn->getPosition() + cocos2d::CCPoint(-50.0f, 0.0f));
    
    // Align Y
    ButtonSprite* alignY = ButtonSprite::create("AlignY", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    alignY->setScale(0.8f);
    CCMenuItemSpriteExtra* alignYBtn = CCMenuItemSpriteExtra::create(
        alignY, nullptr, this, menu_selector(EditorPauseLayer::onAlignY));
    bottomMenu->addChild(alignYBtn);
    alignYBtn->setPosition(buildBtn->getPosition() + cocos2d::CCPoint(-50.0f, 0.0f));
    
    // Select All
    ButtonSprite* selectAll = ButtonSprite::create("Select\nAll", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    selectAll->setScale(0.8f);
    CCMenuItemSpriteExtra* selectAllBtn = CCMenuItemSpriteExtra::create(
        selectAll, nullptr, this, menu_selector(EditorPauseLayer::onSelectAll));
    bottomMenu->addChild(selectAllBtn);
    selectAllBtn->setPosition(unlockBtn->getPosition() + cocos2d::CCPoint(-50.0f, 30.0f));
    
    // Select All Left
    ButtonSprite* selectLeft = ButtonSprite::create("Select\nAll Left", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    selectLeft->setScale(0.8f);
    CCMenuItemSpriteExtra* selectLeftBtn = CCMenuItemSpriteExtra::create(
        selectLeft, nullptr, this, menu_selector(EditorPauseLayer::onSelectAllLeft));
    bottomMenu->addChild(selectLeftBtn);
    selectLeftBtn->setPosition(selectAllBtn->getPosition() + cocos2d::CCPoint(0, -30.0f));
    
    // Select All Right
    ButtonSprite* selectRight = ButtonSprite::create("Select\nAll Right", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    selectRight->setScale(0.8f);
    CCMenuItemSpriteExtra* selectRightBtn = CCMenuItemSpriteExtra::create(
        selectRight, nullptr, this, menu_selector(EditorPauseLayer::onSelectAllRight));
    bottomMenu->addChild(selectRightBtn);
    selectRightBtn->setPosition(selectLeftBtn->getPosition() + cocos2d::CCPoint(0, -30.0f));
    
    // New Group X
    ButtonSprite* newGroupX = ButtonSprite::create("New\nGroupX", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    newGroupX->setScale(0.8f);
    CCMenuItemSpriteExtra* newGroupXBtn = CCMenuItemSpriteExtra::create(
        newGroupX, nullptr, this, menu_selector(EditorPauseLayer::onNewGroupX));
    bottomMenu->addChild(newGroupXBtn);
    newGroupXBtn->setPosition(selectRightBtn->getPosition() + cocos2d::CCPoint(0, -30.0f));
    
    // New Group Y
    ButtonSprite* newGroupY = ButtonSprite::create("New\nGroupY", 0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    newGroupY->setScale(0.8f);
    CCMenuItemSpriteExtra* newGroupYBtn = CCMenuItemSpriteExtra::create(
        newGroupY, nullptr, this, menu_selector(EditorPauseLayer::onNewGroupY));
    bottomMenu->addChild(newGroupYBtn);
    newGroupYBtn->setPosition(newGroupXBtn->getPosition() + cocos2d::CCPoint(0, -30.0f));
    
    // Toggle buttons on left side
    float leftX = cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f;
    float bottomY = cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 24.0f;
    float toggleOffset = 24.0f;
    
    // Ignore Damage
    GameToolbox::createToggleButton(
        "Ignore Damage",
        menu_selector(EditorPauseLayer::toggleIgnoreDamage),
        0,
        GameManager::sharedState()->getGameVariable("0009"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 0 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Select Filter
    GameToolbox::createToggleButton(
        "Select Filter",
        menu_selector(EditorPauseLayer::toggleSelectFilter),
        0,
        GameManager::sharedState()->getGameVariable("0064"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 1 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Show Grid
    GameToolbox::createToggleButton(
        "Show Grid",
        menu_selector(EditorPauseLayer::toggleEditorGrid),
        0,
        GameManager::sharedState()->getGameVariable("0038"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 2 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Show Object Info
    GameToolbox::createToggleButton(
        "Show Object Info",
        menu_selector(EditorPauseLayer::toggleShowObjectInfo),
        0,
        GameManager::sharedState()->getGameVariable("0041"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 3 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Show Ground
    GameToolbox::createToggleButton(
        "Show Ground",
        menu_selector(EditorPauseLayer::toggleEditorGround),
        0,
        GameManager::sharedState()->getGameVariable("0037"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 4 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Preview Shaders
    GameToolbox::createToggleButton(
        "Preview Shaders",
        menu_selector(EditorPauseLayer::togglePreviewShaders),
        0,
        GameManager::sharedState()->getGameVariable("0158"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 5 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Preview Particles
    GameToolbox::createToggleButton(
        "Preview Particles",
        menu_selector(EditorPauseLayer::togglePreviewParticles),
        0,
        GameManager::sharedState()->getGameVariable("0117"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 6 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Preview Animations
    GameToolbox::createToggleButton(
        "Preview Animations",
        menu_selector(EditorPauseLayer::togglePreviewAnim),
        0,
        GameManager::sharedState()->getGameVariable("0118"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 7 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Preview Mode
    GameToolbox::createToggleButton(
        "Preview Mode",
        menu_selector(EditorPauseLayer::toggleEditorColorMode),
        0,
        GameManager::sharedState()->getGameVariable("0036"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 8 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Hide Invisible
    GameToolbox::createToggleButton(
        "Hide Invisible",
        menu_selector(EditorPauseLayer::toggleHideInvisible),
        0,
        GameManager::sharedState()->getGameVariable("0121"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 9 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Show Hitboxes
    GameToolbox::createToggleButton(
        "Show Hitboxes",
        menu_selector(EditorPauseLayer::toggleDebugDraw),
        0,
        GameManager::sharedState()->getGameVariable("0045"),
        bottomMenu,
        cocos2d::CCPoint(leftX, bottomY + 10 * toggleOffset),
        this, this, 0.55f, 0.42f, 85.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt", false, false, false
    );
    
    // Options button
    cocos2d::CCSprite* optionsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    optionsSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* optionsBtn = CCMenuItemSpriteExtra::create(
        optionsSprite, nullptr, this, menu_selector(EditorPauseLayer::onOptions));
    bottomMenu->addChild(optionsBtn);
    optionsBtn->setPosition(buildBtn->getPosition() + cocos2d::CCPoint(0, 60.0f));
}

void EditorPauseLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    int tag = layer->getTag(layer);
    
    if (tag == 1 && btn2)
    {
        onExitEditor(nullptr);
    }
    else if (tag == 2 && btn2)
    {
        doResetUnused();
    }
    else if (tag == 3 && btn2)
    {
        saveLevel();
    }
}

void EditorPauseLayer::keyBackClicked()
{
    onResume(nullptr);
}

void EditorPauseLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    if (key == CONTROLLER_Start)
        keyBackClicked();
}

void EditorPauseLayer::doResetUnused()
{
    LevelEditorLayer::resetUnusedColorChannels(m_editorLayer);
}

void EditorPauseLayer::onAlignX(cocos2d::CCObject* sender)
{
    EditorUI* ui = m_editorLayer->m_editorUI;
    cocos2d::CCArray* objects = ui->getSelectedObjects();
    ui->alignObjects(objects, false);
}

void EditorPauseLayer::onAlignY(cocos2d::CCObject* sender)
{
    EditorUI* ui = m_editorLayer->m_editorUI;
    cocos2d::CCArray* objects = ui->getSelectedObjects();
    ui->alignObjects(objects, true);
}

void EditorPauseLayer::onBuildHelper(cocos2d::CCObject* sender)
{
    EditorUI::dynamicGroupUpdate(m_editorLayer->m_editorUI, false);
}

void EditorPauseLayer::onCopyWColor(cocos2d::CCObject* sender)
{
    EditorUI::doCopyObjects(m_editorLayer->m_editorUI, true);
}

void EditorPauseLayer::onCreateExtras(cocos2d::CCObject* sender)
{
    EditorUI* ui = m_editorLayer->m_editorUI;
    cocos2d::CCArray* objects = ui->getSelectedObjects();
    ui->createRockBases(objects);
}

void EditorPauseLayer::onCreateLoop(cocos2d::CCObject* sender)
{
    EditorUI::createLoop(m_editorLayer->m_editorUI);
}

void EditorPauseLayer::onCreateTemplate(cocos2d::CCObject* sender)
{
    // Implementation would create a template from selected objects
}

void EditorPauseLayer::onExitEditor(cocos2d::CCObject* sender)
{
    FMODAudioEngine::sharedEngine()->unloadAllEffects();
    CCBlockLayer::decrementForcePrio(this);
    GameManager::sharedState()->returnToLastScene(m_editorLayer->m_level);
}

void EditorPauseLayer::onExitNoSave(cocos2d::CCObject* sender)
{
    if (m_saved)
        return;
    
    if (m_editorLayer->m_level->m_levelType == Editor)
    {
        FLAlertLayer* alert = FLAlertLayer::create(
            this,
            "Exit",
            "<cy>Exit</c> without saving? All unsaved changes will be <cr>lost</c>!",
            "No",
            "Yes"
        );
        alert->setTag(1);
        alert->show();
    }
    else
    {
        onExitEditor(nullptr);
    }
}

void EditorPauseLayer::onHelp(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->openEditorGuide();
}

void EditorPauseLayer::onKeybindings(cocos2d::CCObject* sender)
{
    KeybindingsLayer* layer = KeybindingsLayer::create();
    layer->show();
}

void EditorPauseLayer::onNewGroupX(cocos2d::CCObject* sender)
{
    EditorUI::assignNewGroups(m_editorLayer->m_editorUI, false);
}

void EditorPauseLayer::onNewGroupY(cocos2d::CCObject* sender)
{
    EditorUI::assignNewGroups(m_editorLayer->m_editorUI, true);
}

void EditorPauseLayer::onOptions(cocos2d::CCObject* sender)
{
    EditorOptionsLayer* layer = EditorOptionsLayer::create();
    layer->show();
}

void EditorPauseLayer::onPasteWColor(cocos2d::CCObject* sender)
{
    EditorUI::doPasteObjects(m_editorLayer->m_editorUI, true);
}

void EditorPauseLayer::onReGroup(cocos2d::CCObject* sender)
{
    EditorUI::dynamicGroupUpdate(m_editorLayer->m_editorUI, true);
}

void EditorPauseLayer::onResetUnusedColors(cocos2d::CCObject* sender)
{
    if (m_saved)
        return;
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Reset Unused Colors",
        "Reset all unused color channels?\n<cy>This will make those channels available as 'next free'</c>",
        "No",
        "Yes"
    );
    alert->setTag(2);
    alert->show();
}

void EditorPauseLayer::onResume(cocos2d::CCObject* sender)
{
    if (m_saved)
        return;
    
    CCBlockLayer::decrementForcePrio(this);
    LevelEditorLayer::updateOptions(m_editorLayer);
    EditorUI::toggleLockUI(m_editorLayer->m_editorUI, false);
    removeMeAndCleanup();
    
    EditorUI* ui = m_editorLayer->m_editorUI;
    if (ui->m_reloadItems)
        ui->recreateButtonTabs();
}

void EditorPauseLayer::onSave(cocos2d::CCObject* sender)
{
    if (m_saved)
        return;
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Save",
        "<cy>Save</c> the level?",
        "No",
        "Yes"
    );
    alert->setTag(3);
    alert->show();
}

void EditorPauseLayer::onSaveAndExit(cocos2d::CCObject* sender)
{
    if (m_saved)
        return;
    
    setKeypadEnabled(false);
    m_saved = true;
    saveLevel();
    onExitEditor(nullptr);
}

void EditorPauseLayer::onSaveAndPlay(cocos2d::CCObject* sender)
{
    if (m_saved)
        return;
    
    setKeypadEnabled(false);
    m_saved = true;
    saveLevel();
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(EditorPauseLayer::playStep2)),
        nullptr
    ));
}

void EditorPauseLayer::onSelectAll(cocos2d::CCObject* sender)
{
    EditorUI::selectAll(m_editorLayer->m_editorUI);
}

void EditorPauseLayer::onSelectAllLeft(cocos2d::CCObject* sender)
{
    EditorUI::selectAllWithDirection(m_editorLayer->m_editorUI, true);
}

void EditorPauseLayer::onSelectAllRight(cocos2d::CCObject* sender)
{
    EditorUI::selectAllWithDirection(m_editorLayer->m_editorUI, false);
}

void EditorPauseLayer::onSong(cocos2d::CCObject* sender)
{
    GameManager* gm = GameManager::sharedState();
    gm->m_showSongMarkers = !gm->m_showSongMarkers;
    updateSongButton();
}

void EditorPauseLayer::onUnlockAllLayers(cocos2d::CCObject* sender)
{
    LevelEditorLayer::unlockAllLayers(m_editorLayer);
}

void EditorPauseLayer::playStep2()
{
    FMODAudioEngine::sharedEngine()->unloadAllEffects();
    FMODAudioEngine::sharedEngine()->loadMusic(
        m_editorLayer->m_level->getAudioFileName(),
        1.0f, 0.0f, 1.0f, 1, false, false, false
    );
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(EditorPauseLayer::playStep3)),
        nullptr
    ));
}

void EditorPauseLayer::playStep3()
{
    CCBlockLayer::decrementForcePrio(this);
    GJGameLoadingLayer* loadingLayer = GJGameLoadingLayer::transitionToLoadingLayer(m_editorLayer->m_level, false);
    
    if (loadingLayer != m_editorLayer->m_loadingLayer)
    {
        if (loadingLayer)
            loadingLayer->retain();
        if (m_editorLayer->m_loadingLayer)
            m_editorLayer->m_loadingLayer->release();
        m_editorLayer->m_loadingLayer = loadingLayer;
    }
}

void EditorPauseLayer::saveLevel()
{
    GJGameLevel* level = m_editorLayer->m_level;
    std::string levelString = LevelEditorLayer::getLevelString(m_editorLayer);
    
    // Check if level changed
    if (level->m_levelString != levelString)
    {
        level->levelWasAltered();
    }
    
    // Compress and save level string
    std::string compressed = cocos2d::ZipUtils::compressString(levelString, false);
    level->m_levelString = compressed;
    
    // Update level metadata
    level->m_twoPlayerMode = m_editorLayer->m_levelSettings->m_twoPlayerMode;
    level->m_audioTrack = m_editorLayer->m_level->m_audioTrack;
    
    // Update length
    float lastX = LevelEditorLayer::getLastObjectX(m_editorLayer);
    float time = m_editorLayer->m_drawGridLayer->timeForPos(cocos2d::CCPoint(lastX + 340.0f, 0.0f));
    int timestamp = m_editorLayer->m_level->m_timestamp;
    if (timestamp > 0 && time < timestamp / 240.0f)
        time = timestamp / 240.0f;
    
    level->m_levelLength = GJGameLevel::getLengthKey((int)time, m_editorLayer->m_levelSettings->m_platformerMode);
    
    // Update object count
    int objCount = m_editorLayer->m_objectCount.random - m_editorLayer->m_objectCount.seed;
    level->m_objectCount.value = objCount;
    level->m_objectCount.seed = (int)(((float)rand() * 4.6566e-10f) * 1000.0f);
    level->m_objectCount.random = objCount + level->m_objectCount.seed;
    
    // Update coins
    level->m_coins = m_editorLayer->m_coinCount.random - m_editorLayer->m_coinCount.seed;
    level->m_lowDetailMode = m_editorLayer->m_hasHighDetail;
    
    // Update high object flags
    if (objCount > 40000)
    {
        if (objCount <= 80000)
            level->m_unlimitedObjectsEnabled = 0;
    }
    else
    {
        level->m_highObjectsEnabled = 0;
    }
    
    // Update working time
    level->m_workingTime += (int)m_editorLayer->m_sessionTime;
    
    // Save locked layers
    level->m_lockedEditorLayers = LevelEditorLayer::getLockedLayers(m_editorLayer);
    
    // Save camera positions
    level->m_savedCameraPositions = LevelEditorLayer::getSavedEditorPositions(m_editorLayer);
    
    // Save preview lock
    level->m_previewLock = m_editorLayer->m_previewPosition;
    
    // Save song IDs
    level->m_songIDs = LevelEditorLayer::getSongIDs(m_editorLayer);
    level->m_sfxIDs = LevelEditorLayer::getSFXIDs(m_editorLayer);
    
    if (level->m_sfxIDs.empty() && level->m_songIDs.empty())
        level->m_songIDs = "";
    
    // Save to local manager
    LocalLevelManager::sharedState()->save();
}

void EditorPauseLayer::toggleDebugDraw(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0045");
}

void EditorPauseLayer::toggleEditorBackground(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0078");
    bool enabled = GameManager::sharedState()->getGameVariable("0078");
    LevelEditorLayer::toggleBackground(m_editorLayer, !enabled);
}

void EditorPauseLayer::toggleEditorColorMode(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0036");
    LevelEditorLayer::updateEditorMode(m_editorLayer);
}

void EditorPauseLayer::toggleEditorGrid(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0038");
}

void EditorPauseLayer::toggleEditorGround(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0037");
    bool enabled = GameManager::sharedState()->getGameVariable("0037");
    LevelEditorLayer::toggleGround(m_editorLayer, enabled);
}

void EditorPauseLayer::toggleEffectDuration(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0058");
}

void EditorPauseLayer::toggleEffectLines(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0043");
}

void EditorPauseLayer::toggleFollowPlayer(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0001");
}

void EditorPauseLayer::toggleGridOnTop(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0039");
    bool onTop = GameManager::sharedState()->getGameVariable("0039");
    cocos2d::CCNode* parent = m_editorLayer->m_drawGridLayer->getParent();
    int zOrder = onTop ? 1399 : -999;
    parent->reorderChild(m_editorLayer->m_drawGridLayer, zOrder);
}

void EditorPauseLayer::toggleHideInvisible(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0121");
}

void EditorPauseLayer::toggleIgnoreDamage(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0009");
}

void EditorPauseLayer::togglePlaytestMusic(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0002");
}

void EditorPauseLayer::togglePreviewAnim(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0118");
    LevelEditorLayer::updatePreviewAnim(m_editorLayer);
}

void EditorPauseLayer::togglePreviewParticles(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0117");
    LevelEditorLayer::updatePreviewParticles(m_editorLayer);
}

void EditorPauseLayer::togglePreviewShaders(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0158");
}

void EditorPauseLayer::toggleRecordOrder(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0104");
}

void EditorPauseLayer::toggleSelectFilter(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0064");
}

void EditorPauseLayer::toggleShowObjectInfo(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0041");
    EditorUI::toggleObjectInfoLabel(m_editorLayer->m_editorUI);
}

void EditorPauseLayer::uncheckAllPortals(cocos2d::CCObject* sender)
{
    if (m_saved)
        return;
    
    cocos2d::CCArray* objects = LevelEditorLayer::getAllObjects(m_editorLayer);
    
    for (unsigned int i = 0; i < objects->count(); i++)
    {
        cocos2d::CCObject* obj = objects->objectAtIndex(i);
        int objID = HIDWORD(obj[18].__vftable);
        
        bool isPortal = (objID == 13 || objID == 47 || objID == 111 ||
                         objID == 660 || objID == 1331 || objID == 1933);
        
        if (isPortal && obj[31].m_nTag)
        {
            m_editorLayer->m_drawGridLayer->removeFromGuides((GameObject*)obj);
            obj[31].m_nTag = 0;
        }
    }
}

void EditorPauseLayer::updateSongButton()
{
    bool showMarkers = GameManager::sharedState()->m_showSongMarkers;
    
    m_guidelinesOffButton->setVisible(showMarkers);
    m_guidelinesOffButton->setEnabled(showMarkers);
    m_guidelinesOnButton->setVisible(!showMarkers);
    m_guidelinesOnButton->setEnabled(!showMarkers);
}

EditorPauseLayer::~EditorPauseLayer()
{
    // Base class handles cleanup
}