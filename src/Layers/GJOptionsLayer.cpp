#include "Layers/GJOptionsLayer.h"
#include "Managers/GameManager.h"
#include "Layers/FLAlertLayer.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GJOptionsLayer* GJOptionsLayer::create(int background) {
    auto layer = new GJOptionsLayer();
    if (layer && layer->init(background)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool GJOptionsLayer::init(int background) {
    if (!SetupTriggerPopup::init(nullptr, 0, 0, 400.0f, 280.0f, background)) {
        return false;
    }
    
    m_noElasticity = true;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    m_variables = cocos2d::CCDictionary::create();
    m_variables->retain();
    
    m_values = cocos2d::CCDictionary::create();
    m_values->retain();
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    setupOptions();
    
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    auto closeItem = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(GJOptionsLayer::onClose));
    closeItem->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeItem);
    cocos2d::CCPoint closePos = cocos2d::CCPoint(
        3.0f - (m_width * 0.5f - winSize.width * 0.5f),
        m_height * 0.5f + winSize.height * 0.5f - 3.0f
    );
    closeItem->setPosition(m_buttonMenu->convertToNodeSpace(closePos));
    
    auto prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevButton = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(GJOptionsLayer::onPrevPage));
    m_buttonMenu->addChild(m_prevButton);
    float screenLeft = director->getScreenLeft();
    m_prevButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(screenLeft + 20.0f, winSize.height * 0.5f)));
    m_prevButton->setSizeMult(1.5f);
    
    auto nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(GJOptionsLayer::onNextPage));
    m_buttonMenu->addChild(m_nextButton);
    float screenRight = director->getScreenRight();
    m_nextButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(screenRight - 20.0f, winSize.height * 0.5f)));
    m_nextButton->setSizeMult(1.5f);
    
    goToPage(0);
    
    return true;
}

int GJOptionsLayer::countForPage(int page) {
    auto key = pageKey(page);
    auto str = static_cast<cocos2d::CCString*>(m_values->objectForKey(key));
    return str ? str->intValue() : 0;
}

void GJOptionsLayer::incrementCountForPage(int page) {
    int count = countForPage(page) + 1;
    auto str = cocos2d::CCString::createWithFormat("%i", count);
    m_values->setObject(str, pageKey(page));
}

const char* GJOptionsLayer::pageKey(int page) {
    return cocos2d::CCString::createWithFormat("page_%i", page)->getCString();
}

const char* GJOptionsLayer::layerKey(int page) {
    return cocos2d::CCString::createWithFormat("layer_%i", page)->getCString();
}

const char* GJOptionsLayer::objectKey(int page) {
    return cocos2d::CCString::createWithFormat("object_%i", page)->getCString();
}

const char* GJOptionsLayer::infoKey(int index) {
    return cocos2d::CCString::createWithFormat("info_%i", index)->getCString();
}

cocos2d::CCLayer* GJOptionsLayer::layerForPage(int page) {
    auto key = layerKey(page);
    auto layer = static_cast<cocos2d::CCLayer*>(m_values->objectForKey(key));
    
    if (!layer) {
        layer = cocos2d::CCLayer::create();
        m_mainLayer->addChild(layer);
        m_values->setObject(layer, key);
    }
    
    return layer;
}

cocos2d::CCArray* GJOptionsLayer::objectsForPage(int page) {
    auto key = objectKey(page);
    auto array = static_cast<cocos2d::CCArray*>(m_values->objectForKey(key));
    
    if (!array) {
        array = cocos2d::CCArray::create();
        m_values->setObject(array, key);
    }
    
    return array;
}

cocos2d::CCPoint GJOptionsLayer::nextPosition(int page) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    cocos2d::CCPoint pos1(winSize.width * 0.5f - 160.0f, winSize.height * 0.5f + 80.0f + m_offset);
    cocos2d::CCPoint pos2(winSize.width * 0.5f + 32.0f, pos1.y);
    
    int count = countForPage(page);
    bool usePos1 = ((count + 1) & 1) == 0;
    cocos2d::CCPoint pos = usePos1 ? pos1 : pos2;
    
    if (count + 1 > 2) {
        float rows = static_cast<float>(count) * 0.5f;
        pos.y -= rows * m_gap;
    }
    
    return pos;
}

void GJOptionsLayer::goToPage(int page) {
    if (page > m_maxPage) {
        page = 0;
    } else if (page < 0) {
        page = m_maxPage;
    }
    
    if (m_page == page) {
        return;
    }
    
    auto oldKey = pageKey(m_page);
    auto oldObjects = static_cast<cocos2d::CCArray*>(m_values->objectForKey(oldKey));
    
    if (oldObjects) {
        for (int i = 0; i < oldObjects->count(); ++i) {
            auto obj = oldObjects->objectAtIndex(i);
            obj->setVisible(false);
        }
    }
    
    auto oldLayerKey = layerKey(m_page);
    auto oldLayer = m_values->objectForKey(oldLayerKey);
    if (oldLayer) {
        oldLayer->setVisible(false);
    }
    
    auto newKey = objectKey(page);
    auto newObjects = static_cast<cocos2d::CCArray*>(m_values->objectForKey(newKey));
    
    if (newObjects) {
        for (int i = 0; i < newObjects->count(); ++i) {
            auto obj = newObjects->objectAtIndex(i);
            obj->setVisible(true);
        }
    }
    
    auto newLayerKey = layerKey(page);
    auto newLayer = m_values->objectForKey(newLayerKey);
    if (newLayer) {
        newLayer->setVisible(true);
    }
    
    m_page = page;
    m_prevButton->setVisible(page > 0);
    m_nextButton->setVisible(page < m_maxPage);
}

void GJOptionsLayer::offsetToNextPage() {
    m_toggleCount = m_togglesPerPage + (m_toggleCount / m_togglesPerPage) * m_togglesPerPage;
}

CCMenuItemToggler* GJOptionsLayer::getToggleButton(int id) {
    auto it = m_toggleButtons.find(id);
    if (it != m_toggleButtons.end()) {
        return it->second;
    }
    return nullptr;
}

void GJOptionsLayer::onToggle(cocos2d::CCObject* sender) {
    int id = sender->getTag();
    
    auto variable = static_cast<cocos2d::CCString*>(m_variables->objectForKey(id));
    if (variable) {
        auto manager = GameManager::sharedState();
        manager->toggleGameVariable(variable->getCString());
        didToggleGV(variable->getCString());
    } else {
        didToggle(id);
    }
}

void GJOptionsLayer::didToggle(int toggle) {
  //Empty func, lol
}

void GJOptionsLayer::didToggleGV(const std::string& variable) {
  //Yes, another empty func
}

void GJOptionsLayer::onInfo(cocos2d::CCObject* sender) {
    int id = sender->getTag();
    auto key = infoKey(id);
    auto str = static_cast<cocos2d::CCString*>(m_values->objectForKey(key));
    
    if (str) {
        auto alert = FLAlertLayer::create(nullptr, "Info", str->getCString());
        alert->show();
    }
}

void GJOptionsLayer::onNextPage(cocos2d::CCObject* sender) {
    goToPage(m_page + 1);
}

void GJOptionsLayer::onPrevPage(cocos2d::CCObject* sender) {
    goToPage(m_page - 1);
}

void GJOptionsLayer::setupOptions() {
}

GJOptionsLayer::~GJOptionsLayer() {
    if (m_variables) {
        m_variables->release();
    }
    if (m_values) {
        m_values->release();
    }
}