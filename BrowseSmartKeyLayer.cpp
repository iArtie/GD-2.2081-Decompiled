#include "BrowseSmartKeyLayer.h"
#include "GJSmartTemplate.h"
#include "GJSmartPrefab.h"
#include "GJSmartBlockPreview.h"
#include "InfoAlertButton.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "TextAlertPopup.h"
#include "GameManager.h"
#include "BrowseSmartTemplateLayer.h"

void BrowseSmartKeyLayer::addChanceToSelected(int chance, bool zero) {
    cocos2d::CCArray* selectedBlocks = getAllSelectedBlocks();
    
    for (unsigned int i = 0; i < selectedBlocks->count(); i++) {
        cocos2d::CCObject* obj = selectedBlocks->objectAtIndex(i);
        int tag = obj->getTag();
        
        GJSmartPrefab* prefab = m_template->getPrefabWithID(m_prefabKey, tag);
        if (prefab) {
            if (!zero) {
                prefab->m_prefabChance += chance;
            } else {
                prefab->m_prefabChance = 0;
            }
        }
    }
    
    updateChanceValues();
}

BrowseSmartKeyLayer* BrowseSmartKeyLayer::create(GJSmartTemplate* smartTemplate, std::string prefabKey) {
    BrowseSmartKeyLayer* layer = new BrowseSmartKeyLayer();
    if (layer && layer->init(smartTemplate, prefabKey)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

void BrowseSmartKeyLayer::createTemplateObjects() {
    cocos2d::CCArray* selectedBlocks = getAllSelectedBlocks();
    
    if (selectedBlocks->count() > 0) {
        for (unsigned int i = 0; i < selectedBlocks->count(); i++) {
            cocos2d::CCObject* obj = selectedBlocks->objectAtIndex(i);
            int tag = obj->getTag();
            createPrefab(m_prefabKey, tag);
        }
        
        std::string message = "Added to editor";
        TextAlertPopup* alert = TextAlertPopup::create(message, 0.5f, 0.6f, 150, "");
        this->addChild(alert, 100);
    }
}

void BrowseSmartKeyLayer::deletedSelectedItems() {
    for (unsigned int i = 0; i < m_pages->count(); i++) {
        cocos2d::CCArray* page = static_cast<cocos2d::CCArray*>(m_pages->objectAtIndex(i));
        
        for (unsigned int j = 0; j < page->count(); ) {
            cocos2d::CCObject* obj = page->objectAtIndex(j);
            GJSmartBlockPreview* preview = static_cast<GJSmartBlockPreview*>(obj);
            
            if (preview->m_selected) {
                int tag = preview->getTag();
                m_template->removePrefab(m_prefabKey, tag);
                page->removeObjectAtIndex(j, true);
            } else {
                j++;
            }
        }
    }
    
    updateChanceValues();
}

cocos2d::CCArray* BrowseSmartKeyLayer::getAllSelectedBlocks() {
    cocos2d::CCArray* result = cocos2d::CCArray::create();
    
    for (unsigned int i = 0; i < m_pages->count(); i++) {
        cocos2d::CCArray* page = static_cast<cocos2d::CCArray*>(m_pages->objectAtIndex(i));
        
        for (unsigned int j = 0; j < page->count(); j++) {
            cocos2d::CCObject* obj = page->objectAtIndex(j);
            GJSmartBlockPreview* preview = static_cast<GJSmartBlockPreview*>(obj);
            
            if (preview->m_selected) {
                result->addObject(preview);
            }
        }
    }
    
    return result;
}

bool BrowseSmartKeyLayer::init(GJSmartTemplate* smartTemplate, std::string prefabKey) {
    if (!FLAlertLayer::init(this, 150)) {
        return false;
    }
    
    m_prefabKey = prefabKey;
    BrowseSmartTemplateLayer::baseSetup(this);
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    if (smartTemplate) {
        m_template = smartTemplate;
        m_template->retain();
    }
    
    // Help button
    std::string helpTitle = "Help";
    std::string helpDesc = "Use <cg>Add</c> to give chance to the selected variation(s).\n"
                           "Click <cy>Zero</c> to reset the chance.\n"
                           "Use <cl>Create</c> to create a specific variation.";
    InfoAlertButton* helpBtn = InfoAlertButton::create(helpTitle, helpDesc, 1.0f);
    m_buttonMenu->addChild(helpBtn);
    helpBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 225.0f - 18.0f, winSize.height * 0.5f + 140.0f - 18.0f)
    ));
    
    if (m_template) {
        cocos2d::CCPoint pos = m_buttonMenu->convertToNodeSpace(
            cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 140.0f - 50.0f)
        );
        
        cocos2d::CCArray* prefabs = m_template->getPrefabs(m_prefabKey);
        int totalChance = m_template->getTotalChanceForPrefab(m_prefabKey);
        
        int prefabCount = prefabs ? prefabs->count() : 0;
        
        if (prefabCount > 0) {
            for (int i = 0; i < prefabCount; i++) {
                GJSmartBlockPreview* preview = GJSmartBlockPreview::create(m_prefabKey);
                
                if (i > 0) {
                    cocos2d::CCObject* obj = prefabs->objectAtIndex(i - 1);
                    preview->addTemplateGuide(obj);
                }
                
                preview->addPreview(m_prefabKey, obj, GameManager::sharedState()->m_levelEditorLayer);
                
                if (totalChance > 0) {
                    preview->addChance(obj[1].m_uReference, totalChance);
                }
                
                preview->setScale(0.5f);
                
                CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(
                    preview, nullptr, this, menu_selector(BrowseSmartKeyLayer::onPrefabObject)
                );
                m_buttonMenu->addChild(menuItem);
                
                preview->setTag(obj->getTag());
                menuItem->setTag(obj->getTag());
                
                int pageIndex = i / 28;
                int posIndex = i % 28;
                float x = (posIndex % 7) * 60.0f - 210.0f + 30.0f;
                float y = floorf(posIndex / 7) * -60.0f;
                
                menuItem->setPosition(pos + cocos2d::CCPoint(x, y));
                BrowseSmartTemplateLayer::addObjectToPage(this, menuItem, pageIndex);
            }
        }
        
        BrowseSmartTemplateLayer::goToPage(this, 0);
        
        // Create button
        cocos2d::CCNode* createBtn = ButtonSprite::create("Create", "goldFont.fnt", "GJ_button_01.png", 30.0f);
        CCMenuItemSpriteExtra* createItem = CCMenuItemSpriteExtra::create(
            createBtn, nullptr, this, menu_selector(BrowseSmartKeyLayer::onButton)
        );
        createItem->setTag(1);
        m_buttonMenu->addChild(createItem);
        
        // Delete button
        cocos2d::CCNode* deleteBtn = ButtonSprite::create("Delete", "goldFont.fnt", "GJ_button_05.png", 30.0f);
        CCMenuItemSpriteExtra* deleteItem = CCMenuItemSpriteExtra::create(
            deleteBtn, nullptr, this, menu_selector(BrowseSmartKeyLayer::onButton)
        );
        deleteItem->setTag(2);
        m_buttonMenu->addChild(deleteItem);
        
        // Zero button
        cocos2d::CCNode* zeroBtn = ButtonSprite::create("Zero", "goldFont.fnt", "GJ_button_02.png", 30.0f);
        CCMenuItemSpriteExtra* zeroItem = CCMenuItemSpriteExtra::create(
            zeroBtn, nullptr, this, menu_selector(BrowseSmartKeyLayer::onButton)
        );
        zeroItem->setTag(3);
        m_buttonMenu->addChild(zeroItem);
        
        // Add button
        cocos2d::CCNode* addBtn = ButtonSprite::create("Add", "goldFont.fnt", "GJ_button_02.png", 30.0f);
        CCMenuItemSpriteExtra* addItem = CCMenuItemSpriteExtra::create(
            addBtn, nullptr, this, menu_selector(BrowseSmartKeyLayer::onButton)
        );
        addItem->setTag(4);
        m_buttonMenu->addChild(addItem);
        
        cocos2d::CCPoint btnPos = m_buttonMenu->convertToNodeSpace(
            cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 140.0f + 24.0f)
        );
        
        createItem->setPosition(btnPos + cocos2d::CCPoint(-100.0f, 0.0f));
        deleteItem->setPosition(btnPos + cocos2d::CCPoint(-50.0f, 0.0f));
        zeroItem->setPosition(btnPos + cocos2d::CCPoint(50.0f, 0.0f));
        addItem->setPosition(btnPos + cocos2d::CCPoint(100.0f, 0.0f));
    }
    
    return true;
}

void BrowseSmartKeyLayer::onBack(cocos2d::CCObject* sender) {
    BrowseSmartTemplateLayer::onClose(this, sender);
    
    SmartBrowseFilter filter = {};
    BrowseSmartTemplateLayer* layer = BrowseSmartTemplateLayer::create(m_template, filter);
    layer->show(layer);
}

void BrowseSmartKeyLayer::onButton(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    
    switch (tag) {
        case 1:
            createTemplateObjects();
            break;
        case 2:
            deletedSelectedItems();
            break;
        case 3:
            addChanceToSelected(0, true);
            break;
        case 4:
            addChanceToSelected(10, false);
            break;
        default:
            break;
    }
}

void BrowseSmartKeyLayer::onPrefabObject(cocos2d::CCObject* sender) {
    GJSmartBlockPreview* preview = static_cast<GJSmartBlockPreview*>(sender);
    preview->toggleSelectItem(!preview->m_selected);
}

void BrowseSmartKeyLayer::updateChanceValues() {
    int totalChance = m_template->getTotalChanceForPrefab(m_prefabKey);
    
    for (unsigned int i = 0; i < m_pages->count(); i++) {
        cocos2d::CCArray* page = static_cast<cocos2d::CCArray*>(m_pages->objectAtIndex(i));
        
        for (unsigned int j = 0; j < page->count(); j++) {
            cocos2d::CCObject* obj = page->objectAtIndex(j);
            GJSmartBlockPreview* preview = static_cast<GJSmartBlockPreview*>(obj);
            
            int tag = preview->getTag();
            GJSmartPrefab* prefab = m_template->getPrefabWithID(m_prefabKey, tag);
            
            if (prefab) {
                preview->addChance(prefab->m_prefabChance, totalChance);
            }
        }
    }
}

BrowseSmartKeyLayer::~BrowseSmartKeyLayer() {
    // Destructor - releases resources
}