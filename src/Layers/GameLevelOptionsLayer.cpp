#include "Layers/GameLevelOptionsLayer.h"
#include "Models/GJGameLevel.h"
#include "Cocos/CCMenuItemToggler.h"

GameLevelOptionsLayer* GameLevelOptionsLayer::create(GJGameLevel* level) {
    GameLevelOptionsLayer* ret = new GameLevelOptionsLayer();
    if (ret && ret->init(level)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void GameLevelOptionsLayer::didToggle(int tag) {
    switch (tag) {
        case 1: // Low Detail Mode
            m_level->m_lowDetailModeToggled = !m_level->m_lowDetailModeToggled;
            break;
            
        case 2: // Disable Shake
            m_level->m_disableShakeToggled = !m_level->m_disableShakeToggled;
            break;
            
        case 3: // Enable CBS
        case 4: // Disable CBS
        {
            int newValue;
            if (tag == 3) {
                newValue = (m_level->m_cbsOverride != 1) ? 1 : 0;
            } else {
                newValue = (m_level->m_cbsOverride == 2) ? 0 : 2;
            }
            m_level->m_cbsOverride = newValue;
            
            CCMenuItemToggler* toggle3 = GJOptionsLayer::getToggleButton(this, 3);
            if (toggle3) {
                CCMenuItemToggler::toggle(toggle3, newValue == 1);
            }
            
            CCMenuItemToggler* toggle4 = GJOptionsLayer::getToggleButton(this, 4);
            if (toggle4) {
                CCMenuItemToggler::toggle(toggle4, newValue == 2);
            }
            break;
        }
            
        default:
            break;
    }
}

bool GameLevelOptionsLayer::init(GJGameLevel* level) {
    m_gap = 40.0f;
    m_maxLabelScale = 0.4f;
    
    if (level) {
        m_level = level;
        m_level->retain();
    }
    
    if (!GJOptionsLayer::init(2)) {
        return false;
    }
    
    SetupTriggerPopup::preSetup(this);
    SetupTriggerPopup::postSetup(this);
    
    return true;
}

void GameLevelOptionsLayer::setupOptions() {
    if (!m_level) {
        return;
    }
    
    GJOptionsLayer::addToggle(this, "Low Detail Mode", 1, 
                              m_level->m_lowDetailModeToggled, "");
    
    GJOptionsLayer::addToggle(this, "Disable Shake", 2, 
                              m_level->m_disableShakeToggled, "");
    
    GJOptionsLayer::addToggle(this, "Enable CBS", 3, 
                              m_level->m_cbsOverride == 1,
                              "Force <cg>enable</c> <cy>Click Between Steps</c> on this level regardless of the global setting.");
    
    GJOptionsLayer::addToggle(this, "Disable CBS", 4, 
                              m_level->m_cbsOverride == 2,
                              "Force <cr>disable</c> <cy>Click Between Steps</c> on this level regardless of the global setting.");
}

GameLevelOptionsLayer::~GameLevelOptionsLayer() {
    if (m_level) {
        m_level->release();
        m_level = nullptr;
    }
}