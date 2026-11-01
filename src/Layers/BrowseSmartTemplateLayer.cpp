#include "Layers/BrowseSmartTemplateLayer.h"
#include "Editor/GJSmartTemplate.h"
#include "Editor/GJSmartPrefab.h"
#include "Editor/GJSmartBlockPreview.h"
#include "UI/InfoAlertButton.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Popups/TextAlertPopup.h"
#include "Managers/GameManager.h"
#include "Editor/EditorUI.h"
#include "Popups/Setup/SetupSmartTemplateLayer.h"
#include "Layers/BrowseSmartKeyLayer.h"
#include "Platform/cocos2d.h"

void BrowseSmartTemplateLayer::addObjectToPage(cocos2d::CCObject* object, int page) {
    while ((unsigned int)m_pages->count() < (unsigned int)(page + 1)) {
        cocos2d::CCArray* newPage = cocos2d::CCArray::create();
        m_pages->addObject(newPage);
    }
    
    cocos2d::CCArray* targetPage = static_cast<cocos2d::CCArray*>(m_pages->objectAtIndex(page));
    targetPage->addObject(object);
}

void BrowseSmartTemplateLayer::addPrefabMenuItem(SmartPrefabResult* smartPrefabResult, int index) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint menuPos = m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 140.0f - 50.0f)
    );
    
    GJSmartBlockPreview* preview = GJSmartBlockPreview::create(smartPrefabResult->m_binaryKey);
    
    float scale = (smartPrefabResult->m_prefabCount > 0) ? 0.5f : 1.0f;
    preview->addTemplateGuide(smartPrefabResult, scale);
    
    if (smartPrefabResult->m_prefabCount > 0) {
        GJSmartPrefab* randomPrefab = m_template->getRandomPrefab(smartPrefabResult->m_binaryKey);
        if (randomPrefab) {
            preview->addPreview(
                smartPrefabResult->m_binaryKey,
                randomPrefab->m_prefabKey,
                GameManager::sharedState()->m_levelEditorLayer
            );
            
            if (smartPrefabResult->m_prefabCount > 1) {
                preview->addCount(smartPrefabResult->m_prefabCount);
            }
        }
    }
    
    preview->setScale(0.5f);
    
    CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(
        preview, nullptr, this, menu_selector(BrowseSmartTemplateLayer::onTemplateObject)
    );
    m_buttonMenu->addChild(menuItem);
    
    int pageIndex = index / 28;
    int pos = index - (28 * pageIndex);
    float x = (pos % 7) * 60.0f - 210.0f + 30.0f;
    float y = floorf(pos / 7) * -60.0f;
    
    menuItem->setPosition(menuPos + cocos2d::CCPoint(x, y));
    addObjectToPage(menuItem, pageIndex);
}

void BrowseSmartTemplateLayer::baseSetup() {
    m_pages = cocos2d::CCArray::create();
    m_pages->retain();
    m_noElasticity = true;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square02.png",
        cocos2d::CCRect(0, 0, 80, 80)
    );
    bg->setContentSize(cocos2d::CCSize(450.0f, 280.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    // Menu
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.7f);
    
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(BrowseSmartTemplateLayer::onClose)
    );
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 225.0f + 6.0f, winSize.height * 0.5f + 140.0f - 6.0f)
    ));
    
    // Previous page button
    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    m_prevPageBtn = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, menu_selector(BrowseSmartTemplateLayer::onClick)
    );
    m_prevPageBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(m_prevPageBtn);
    m_prevPageBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(25.0f, winSize.height * 0.5f)
    ));
    m_prevPageBtn->setTag(1);
    
    // Next page button
    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextSprite->setFlipX(true);
    m_nextPageBtn = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(BrowseSmartTemplateLayer::onClick)
    );
    m_nextPageBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(m_nextPageBtn);
    m_nextPageBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width - 25.0f, winSize.height * 0.5f)
    ));
    m_nextPageBtn->setTag(2);
}

void BrowseSmartTemplateLayer::createDots() {
    cocos2d::CCSpriteBatchNode* batchNode = cocos2d::CCSpriteBatchNode::create("smallDot.png", 5);
    m_mainLayer->addChild(batchNode, 5);
    
    m_dotsArray = cocos2d::CCArray::create();
    m_dotsArray->retain();
    
    for (unsigned int i = 0; i < m_pages->count(); i++) {
        cocos2d::CCSprite* dot = cocos2d::CCSprite::create("smallDot.png");
        dot->setScale(0.8f);
        batchNode->addChild(dot);
        m_dotsArray->addObject(dot);
    }
}

void BrowseSmartTemplateLayer::createPrefab(std::string key, int id) {
    EditorUI* editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
    editorUI->createPrefab(m_template, key, id);
}

void BrowseSmartTemplateLayer::goToPage(int page) {
    if (page < 0) {
        m_page = m_pages->count() - 1;
    } else if (page >= (int)m_pages->count()) {
        m_page = 0;
    } else {
        m_page = page;
    }
    
    for (unsigned int i = 0; i < m_pages->count(); i++) {
        bool isCurrentPage = (m_page == (int)i);
        cocos2d::CCArray* pageArray = static_cast<cocos2d::CCArray*>(m_pages->objectAtIndex(i));
        
        for (unsigned int j = 0; j < pageArray->count(); j++) {
            cocos2d::CCObject* obj = pageArray->objectAtIndex(j);
            obj->setVisible(isCurrentPage);
        }
    }
    
    if (m_pages->count() <= 1) {
        m_prevPageBtn->setVisible(false);
        m_nextPageBtn->setVisible(false);
    } else {
        updateDots();
    }
}

void BrowseSmartTemplateLayer::keyBackClicked() {
    onBack(nullptr);
}

void BrowseSmartTemplateLayer::onBack(cocos2d::CCObject* sender) {
    SetupSmartTemplateLayer* layer = SetupSmartTemplateLayer::create(m_template);
    layer->show(layer);
    onClose(sender);
}

void BrowseSmartTemplateLayer::onClick(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    if (tag == 1) {
        goToPage(m_page - 1);
    } else if (tag == 2) {
        goToPage(m_page + 1);
    }
}

void BrowseSmartTemplateLayer::onClose(cocos2d::CCObject* sender) {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);
    this->setKeypadEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void BrowseSmartTemplateLayer::onTemplateObject(cocos2d::CCObject* sender) {
    GJSmartBlockPreview* preview = static_cast<GJSmartBlockPreview*>(sender);
    std::string key = preview->m_prefabKey;
    
    cocos2d::CCArray* prefabs = m_template->getPrefabs(key);
    
    if (prefabs && prefabs->count() > 0) {
        onClose(nullptr);
        BrowseSmartKeyLayer* layer = BrowseSmartKeyLayer::create(m_template, key);
        layer->m_templatePage = m_page;
        layer->show(layer);
    } else {
        createPrefab(key, 0);
        
        std::string message = "Added to editor";
        TextAlertPopup* alert = TextAlertPopup::create(message, 0.5f, 0.6f, 150, "");
        this->addChild(alert, 100);
    }
}

void BrowseSmartTemplateLayer::updateDots() {
    if (!m_dotsArray) {
        createDots();
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float totalPages = static_cast<float>(m_pages->count());
    
    for (unsigned int i = 0; i < m_pages->count(); i++) {
        if (i < m_dotsArray->count()) {
            cocos2d::CCObject* dot = m_dotsArray->objectAtIndex(i);
            
            float x = winSize.width * 0.5f + (static_cast<float>(i) - (totalPages - 1.0f) * 0.5f) * 16.0f;
            float y = winSize.height * 0.5f - 140.0f + 15.0f;
            dot->setPosition(cocos2d::CCPoint(x, y));
            
            if (m_page == (int)i) {
                dot->setColor(cocos2d::ccc3(255, 255, 255));
            } else {
                dot->setColor(cocos2d::ccc3(125, 125, 125));
            }
        }
    }
}

BrowseSmartTemplateLayer::~BrowseSmartTemplateLayer() {
    CC_SAFE_RELEASE_NULL(m_pages);
    CC_SAFE_RELEASE_NULL(m_dotsArray);
}