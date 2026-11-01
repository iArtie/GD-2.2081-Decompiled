#include "Layers/GraphicsReloadLayer.h"
#include "Managers/GameManager.h"
#include "Utils/PlatformToolbox.h"

void GraphicsReloadLayer::performReload() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = cocos2d::CCScene::create();
    director->replaceScene(scene);
    
    PlatformToolbox::updateWindowedSize(m_resolution.width, m_resolution.height);
    
    auto oldQuality = director->m_eTextureQuality;
    director->updateContentScale(m_quality);
    auto newQuality = director->m_eTextureQuality;
    
    auto manager = GameManager::sharedState();
    bool isFullscreen = manager->getGameVariable("0025");
    
    if (m_fullscreen != isFullscreen ||
        (m_fullscreen && (m_borderless != manager->getGameVariable("0170") ||
                          m_fix != manager->getGameVariable("0175")))) {
        manager->setGameVariable("0025", !m_fullscreen);
        manager->setGameVariable("0170", m_borderless);
        manager->setGameVariable("0175", m_fix);
        manager->switchScreenMode(m_fullscreen, m_borderless, m_fix, true);
    } else {
        manager->setGameVariable("0170", m_borderless);
        manager->setGameVariable("0175", m_fix);
        
        if (m_changedResolution && !m_fullscreen) {
            PlatformToolbox::resizeWindow(m_resolution.width, m_resolution.height);
        }
        
        if (oldQuality == newQuality) {
            manager->queueReloadMenu();
        } else {
            manager->reloadAll(false, false, false);
        }
    }
    
    if (manager->getGameVariable("0115")) {
        director->toggleShowFPS(true, "chatFont.fnt", cocos2d::CCPoint(0.0f, 0.0f));
    }
}

GraphicsReloadLayer::~GraphicsReloadLayer() {
}