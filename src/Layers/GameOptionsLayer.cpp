// GameOptionsLayer.cpp
#include "Layers/GameOptionsLayer.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Layers/PlayLayer.h"
#include "Layers/UIOptionsLayer.h"
#include "Layers/UIPOptionsLayer.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Utils/GameToolbox.h"

GameOptionsLayer* GameOptionsLayer::create(GJBaseGameLayer* baseGameLayer) {
    GameOptionsLayer* layer = new GameOptionsLayer();
    if (layer && layer->init(baseGameLayer)) {
        layer->autorelease();
        return layer;
    }
    if (layer) delete layer;
    return nullptr;
}

bool GameOptionsLayer::init(GJBaseGameLayer* baseGameLayer) {
    m_baseGameLayer = baseGameLayer;
    m_gap = 40.0f;
    m_maxLabelScale = 0.4f;
    
    if (!GJOptionsLayer::init(this, 2)) {
        return false;
    }
    
    SetupTriggerPopup::preSetup(this);
    SetupTriggerPopup::postSetup(this);
    return true;
}

void GameOptionsLayer::setupOptions() {
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = director->getWinSize();
    
    m_togglesPerPage = 12;
    m_offset = 32.0f;
    m_gap = 37.0f;
    
    // Auto-Retry
    bool autoRetry = GameManager::sharedState()->getGameVariable("0026");
    GJOptionsLayer::addToggle(this, "Auto-Retry", 1, autoRetry, "");
    
    // Auto-Checkpoints
    bool autoCheckpoints = GameManager::sharedState()->getGameVariable("0027");
    GJOptionsLayer::addToggle(this, "Auto-Checkpoints", 2, autoCheckpoints, "");
    
    // Show Progress Bar
    GJOptionsLayer::addToggle(this, "Show Progress Bar", 3, 
                              GameManager::sharedState()->m_showProgressBar, "");
    
    // Show Percentage
    bool showPercentage = GameManager::sharedState()->getGameVariable("0040");
    GJOptionsLayer::addToggle(this, "Show Percentage", 4, showPercentage, "");
    
    // Show Time
    bool showTime = GameManager::sharedState()->getGameVariable("0145");
    GJOptionsLayer::addToggle(this, "Show Time", 7, showTime, "");
    
    // Audio Visualizer
    bool audioVis = GameManager::sharedState()->getGameVariable("0144");
    GJOptionsLayer::addToggle(this, "Audio Visualizer", 5, audioVis, "");
    
    // Show Info Label
    bool infoLabel = GameManager::sharedState()->getGameVariable("0109");
    GJOptionsLayer::addToggle(this, "Show Info Label", 6, infoLabel, "");
    
    // Disable Checkpoints
    bool noCheckpoints = GameManager::sharedState()->getGameVariable("0146");
    GJOptionsLayer::addToggle(this, "Disable Checkpoints", 8, noCheckpoints, "");
    
    // Show Hitboxes
    bool showHitboxes = GameManager::sharedState()->getGameVariable("0166");
    GJOptionsLayer::addToggle(this, "Show Hitboxes", 9, showHitboxes, 
                              "Shows hitboxes while in practice mode.");
    
    // Hitbox On Death
    bool hitboxDeath = GameManager::sharedState()->getGameVariable("0179");
    GJOptionsLayer::addToggle(this, "Hitbox On Death", 11, hitboxDeath,
                              "Shows hitboxes upon death in both normal and practice mode.");
    
    // Practice Music Sync
    GJOptionsLayer::nextPosition(this, 0);
    bool musicSyncEnabled = GameStatsManager::sharedState()->isItemEnabled(GJItem, 17);
    
    CCMenuItemToggler* syncToggle = GameToolbox::createToggleButton(
        "Practice Music Sync",
        (cocos2d::SEL_MenuHandler)&GameOptionsLayer::onPracticeMusicSync,
        0,
        musicSyncEnabled,
        m_buttonMenu,
        cocos2d::CCPoint(8.0f, 0.0f),
        this,
        m_mainLayer,
        0.8f,
        m_maxLabelScale,
        m_maxLabelWidth
    );
    
    if (!GameStatsManager::sharedState()->isItemUnlocked(GJItem, 17)) {
        syncToggle->m_notClickable = true;
        cocos2d::ccColor3B gray(150, 150, 150);
        syncToggle->activeItem()->setColor(gray);
    }
    
    // Ignore Damage (only for editor levels in playlayer)
    if (GameManager::sharedState()->m_playLayer &&
        GameManager::sharedState()->m_playLayer->m_level &&
        GameManager::sharedState()->m_playLayer->m_level->m_levelType == Editor) {
        
        m_toggleCount++;
        GJOptionsLayer::incrementCountForPage(this, 0);
        cocos2d::CCLayer* layer = GJOptionsLayer::layerForPage(this, 0);
        GJOptionsLayer::nextPosition(this, 0);
        
        bool ignoreDamage = GameManager::sharedState()->getGameVariable("0173");
        
        CCMenuItemToggler* ignoreToggle = GameToolbox::createToggleButton(
            "Ignore Damage",
            (cocos2d::SEL_MenuHandler)&GJOptionsLayer::onToggle,
            0,
            ignoreDamage,
            m_buttonMenu,
            cocos2d::CCPoint(8.0f, 0.0f),
            this,
            layer,
            0.8f,
            0.5f,
            m_maxLabelWidth,
            "goldFont.fnt"
        );
        
        ignoreToggle->setTag(10);
        ignoreToggle->setSizeMult(1.0f);
        
        cocos2d::CCArray* objects = GJOptionsLayer::objectsForPage(this, 0);
        objects->addObject(ignoreToggle);
        ignoreToggle->setVisible(false);
        layer->setVisible(false);
    }
    
    // Platformer UI button
    cocos2d::CCPoint pos = cocos2d::CCPoint(winSize.width * 0.5f, 
                                          26.0f - (m_height * 0.5f - winSize.height * 0.5f));
    pos = m_buttonMenu->convertToNodeSpace(pos);
    
    cocos2d::CCNode* platformerBtn = ButtonSprite::create("Platformer UI", "bigFont.fnt");
    platformerBtn->setScale(0.7f);
    CCMenuItemSpriteExtra* platformerItem = CCMenuItemSpriteExtra::create(
        platformerBtn, nullptr, this, (cocos2d::SEL_MenuHandler)&GameOptionsLayer::onUIOptions);
    m_buttonMenu->addChild(platformerItem);
    platformerItem->setPosition(pos + cocos2d::CCPoint(-80.0f, 0.0f));
    
    // Practice UI button
    cocos2d::CCNode* practiceBtn = ButtonSprite::create("Practice UI", "bigFont.fnt");
    practiceBtn->setScale(0.7f);
    CCMenuItemSpriteExtra* practiceItem = CCMenuItemSpriteExtra::create(
        practiceBtn, nullptr, this, (cocos2d::SEL_MenuHandler)&GameOptionsLayer::onUIPOptions);
    m_buttonMenu->addChild(practiceItem);
    practiceItem->setPosition(pos + cocos2d::CCPoint(80.0f, 0.0f));
}

void GameOptionsLayer::didToggle(int tag) {
    switch (tag) {
        case 1:
            GameManager::sharedState()->toggleGameVariable("0026");
            break;
        case 2:
            GameManager::sharedState()->toggleGameVariable("0027");
            break;
        case 3:
            GameManager::sharedState()->m_showProgressBar = !GameManager::sharedState()->m_showProgressBar;
            m_baseGameLayer->toggleProgressbar();
            break;
        case 4:
            GameManager::sharedState()->toggleGameVariable("0040");
            break;
        case 5:
            GameManager::sharedState()->toggleGameVariable("0144");
            m_baseGameLayer->toggleAudioVisualizer(GameManager::sharedState()->getGameVariable("0144"));
            break;
        case 6:
            GameManager::sharedState()->toggleGameVariable("0109");
            m_baseGameLayer->toggleInfoLabel();
            break;
        case 7:
            GameManager::sharedState()->toggleGameVariable("0145");
            m_baseGameLayer->toggleProgressbar();
            break;
        case 8:
            GameManager::sharedState()->toggleGameVariable("0146");
            m_baseGameLayer->removeAllCheckpoints();
            break;
        case 9:
            GameManager::sharedState()->toggleGameVariable("0166");
            if (GameManager::sharedState()->m_playLayer) {
                PlayLayer::updateDebugDrawSettings(GameManager::sharedState()->m_playLayer);
            }
            break;
        case 10:
            bool ignoreDamage = GameManager::sharedState()->toggleGameVariable("0173");
            if (GameManager::sharedState()->m_playLayer) {
                PlayLayer::toggleIgnoreDamage(GameManager::sharedState()->m_playLayer, ignoreDamage);
            }
            break;
        case 11:
            GameManager::sharedState()->toggleGameVariable("0179");
            if (GameManager::sharedState()->m_playLayer) {
                PlayLayer::updateDebugDrawSettings(GameManager::sharedState()->m_playLayer);
            }
            break;
    }
}

void GameOptionsLayer::onPracticeMusicSync(cocos2d::CCObject* sender) {
    GameStatsManager* stats = GameStatsManager::sharedState();
    if (stats->isItemUnlocked(GJItem, 17)) {
        bool enabled = stats->isItemEnabled(GJItem, 17);
        stats->toggleEnableItem(GJItem, 17, !enabled);
        m_baseGameLayer->toggleMusicInPractice();
    } else {
        showPracticeMusicSyncUnlockInfo();
    }
}

void GameOptionsLayer::showPracticeMusicSyncUnlockInfo() {
    std::string text, speaker;
    int dialogType = 2;
    int dialogIndex = 0;
    
    switch (m_practiceDialogIndex) {
        case 0:
            text = "You need a <cg>Music Unlocker</c> to play <cy>normal music</c> in <cl>practice mode</c>.";
            speaker = "";
            dialogIndex = 10;
            break;
        case 1:
            text = "I think the <cl>Diamond Shopkeeper</c> has one, <d030><co>weird guy</c>...";
            speaker = "Scratch";
            dialogIndex = 26;
            break;
        case 2:
            text = "<cl>SpaRkle gnAsH Glorp sHiMmerRr crUnCHle bLinGz...</c>";
            speaker = "Diamond Shopkeeper";
            dialogIndex = 40;
            break;
        case 3:
            text = "See what I mean?.<d010>.<d010>.<d010>";
            speaker = "Scratch";
            dialogIndex = 12;
            break;
        case 4:
            text = "<cl>ShiM gNaw glEaM.<d010>.<d010>.<d010></c> <cy><i050><s260>bRinG</s> <s260>ShiNy!</c></s></i>";
            speaker = "Diamond Shopkeeper";
            dialogIndex = 41;
            break;
        case 5:
            text = "Looks to me like you had enough <cy>shiny</c>.<d010>.<d010>.<d010>";
            speaker = "Scratch";
            dialogIndex = 11;
            break;
        case 6:
            text = "<cl>ShiMmer.<d010>.<d010>.<d010> sHim.<d010>.<d010>.<d010> bLiNg<d020> sNap.<d010>.<d010>.<d010> <i050><s260>HA!</s></i></c>";
            speaker = "Diamond Shopkeeper";
            dialogIndex = 42;
            break;
        case 7:
            text = ".<d010>.<d010>.<d010>";
            speaker = "Scratch";
            dialogIndex = 12;
            break;
        default:
            text = "You need a <cg>Music Unlocker</c> to play <cy>normal music</c> in <cl>practice mode</c>.";
            speaker = "";
            dialogIndex = 10;
            break;
    }
    
    DialogObject* dialog = DialogObject::create(speaker, text, dialogIndex, 1.0f, 0, cocos2d::ccColor3B(255, 255, 255));
    DialogLayer* layer = DialogLayer::create(dialog, dialogType);
    layer->addToMainScene();
    layer->updateChatPlacement(Center);
    layer->animateIn(FromCenter);
    
    m_practiceDialogIndex++;
    if (m_practiceDialogIndex > 7) m_practiceDialogIndex = 0;
}

void GameOptionsLayer::onUIOptions(cocos2d::CCObject* sender) {
    UIOptionsLayer* layer = UIOptionsLayer::create(nullptr);
    layer->show();
}

void GameOptionsLayer::onUIPOptions(cocos2d::CCObject* sender) {
    UIPOptionsLayer* layer = UIPOptionsLayer::create();
    layer->show();
}

GameOptionsLayer::~GameOptionsLayer() {
    // Base class destructor handles cleanup
}