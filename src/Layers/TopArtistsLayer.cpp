#include "Layers/TopArtistsLayer.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "UI/LoadingCircle.h"
#include "Layers/GJCommentListLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/CustomListView.h"

TopArtistsLayer* TopArtistsLayer::create() {
    TopArtistsLayer* returnVal = new TopArtistsLayer();
    if (returnVal->init()) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool TopArtistsLayer::init() {
    if (!FLAlertLayer::init(150))
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square02.png");
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    bg->setPosition(centerX, centerY);
    m_mainLayer->addChild(bg, -1);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Top Artists", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel, 20);
    titleLabel->setPosition(centerX, centerY + 145.0f - 18.0f);
    titleLabel->setScale(0.6f);

    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
        nullptr, menu_selector(TopArtistsLayer::onClose));
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setSizeMult(1.6f);
    closeBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, &cocos2d::CCPoint(centerX - 220.0f + 10.0f, centerY + 145.0f - 10.0f)));

    cocos2d::CCMenu* arrowMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(arrowMenu, 10);

    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    prevSprite->setScale(0.8f);
    m_prevButton = CCMenuItemSpriteExtra::create(prevSprite, nullptr, menu_selector(TopArtistsLayer::onPrevPage));
    m_prevButton->setSizeMult(1.2f);
    arrowMenu->addChild(m_prevButton);
    m_prevButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        arrowMenu, &cocos2d::CCPoint(centerX - 220.0f + 22.0f, centerY)));

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextSprite->setScale(0.8f);
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(nextSprite, nullptr, menu_selector(TopArtistsLayer::onNextPage));
    m_nextButton->setSizeMult(1.2f);
    arrowMenu->addChild(m_nextButton);
    m_nextButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        arrowMenu, &cocos2d::CCPoint(centerX + 220.0f - 22.0f, centerY)));

    m_prevButton->setVisible(false);
    m_nextButton->setVisible(false);

    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->retain();
    m_loadingCircle->show();
    m_loadingCircle->setVisible(false);

    m_pageInfoLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_pageInfoLabel);
    m_pageInfoLabel->setPosition(centerX + 220.0f - 8.0f, centerY + 145.0f - 8.0f);
    m_pageInfoLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));

    std::string key = GameLevelManager::sharedState()->getTopArtistsKey(0);
    cocos2d::CCArray* stored = GameLevelManager::sharedState()->getStoredOnlineLevels(key.c_str());
    if (stored) {
        std::string pageInfo = GameLevelManager::sharedState()->getPageInfo(key.c_str());
        setupPageInfo(pageInfo, key.c_str());
        loadListFinished(stored, key.c_str());
    } else {
        GameLevelManager::sharedState()->makeTimeStamp("upd_artists");
        m_loadingCircle->setVisible(true);
        setupLeaderboard(nullptr);
        GameLevelManager::sharedState()->m_onlineListDelegate = this;
        GameLevelManager::sharedState()->getTopArtists(0, 0);
    }

    return true;
}

bool TopArtistsLayer::isCorrect(const char* key) {
    return true;
}

void TopArtistsLayer::keyBackClicked() {
    onClose(nullptr);
}

void TopArtistsLayer::loadListFailed(const char* key) {
    if (isCorrect(key))
        m_loadingCircle->setVisible(false);
}

void TopArtistsLayer::loadListFinished(cocos2d::CCArray* objects, const char* key) {
    if (objects && isCorrect(key)) {
        if (m_topArtists)
            m_topArtists->release();
        m_topArtists = objects;
        m_topArtists->retain();
        setupLeaderboard(m_topArtists);
        m_loadingCircle->setVisible(false);
    }
}

void TopArtistsLayer::loadPage(int page) {
    m_page = page;
    std::string key = GameLevelManager::sharedState()->getTopArtistsKey(m_page);
    std::string nextKey = GameLevelManager::sharedState()->getTopArtistsKey(m_page + 1);

    m_loadingCircle->setVisible(true);
    m_prevButton->setVisible(m_page > 0);
    m_nextButton->setVisible(GameLevelManager::sharedState()->getStoredOnlineLevels(nextKey.c_str()) != nullptr);

    cocos2d::CCArray* stored = GameLevelManager::sharedState()->getStoredOnlineLevels(key.c_str());
    if (!stored) {
        GameLevelManager::sharedState()->makeTimeStamp("upd_artists");
        m_loadingCircle->setVisible(true);
        setupLeaderboard(nullptr);
        GameLevelManager::sharedState()->m_onlineListDelegate = this;
        GameLevelManager::sharedState()->getTopArtists(m_page, m_itemCount);
        m_pageInfoLabel->setString(" ");
    } else {
        std::string pageInfo = GameLevelManager::sharedState()->getPageInfo(key.c_str());
        setupPageInfo(pageInfo, key.c_str());
        loadListFinished(stored, key.c_str());
    }
}

void TopArtistsLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void TopArtistsLayer::onNextPage(cocos2d::CCObject* sender) {
    loadPage(m_page + 1);
}

void TopArtistsLayer::onPrevPage(cocos2d::CCObject* sender) {
    loadPage(m_page - 1);
}

void TopArtistsLayer::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void TopArtistsLayer::setupLeaderboard(cocos2d::CCArray* artists) {
    if (m_listLayer) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    CustomListView* listView = artists ? CustomListView::create(artists, BoomListType::Default, 220.0f, 340.0f) : nullptr;
    cocos2d::ccColor4B color = cocos2d::ccc4(60, 60, 60, 255);
    m_listLayer = GJCommentListLayer::create(listView, "", color, 340.0f, 220.0f, true);
    m_mainLayer->addChild(m_listLayer);
    m_listLayer->setPosition((winSize.width - 340.0f) * 0.5f, (winSize.height - 220.0f) * 0.5f - 10.0f);
}

void TopArtistsLayer::setupPageInfo(std::string info, const char* key) {
    if (!isCorrect(key))
        return;

    std::vector<std::string> parts = GameToolbox::splitString(info, ":");
    cocos2d::CCArray* array = cocos2d::CCArray::create();
    for (auto& part : parts) {
        if (!part.empty())
            array->addObject(cocos2d::CCString::create(part));
    }

    if (array->count() > 2) {
        m_itemCount = ((cocos2d::CCString*)array->objectAtIndex(0))->intValue();
        m_pageStartIdx = ((cocos2d::CCString*)array->objectAtIndex(1))->intValue();
        m_pageEndIdx = ((cocos2d::CCString*)array->objectAtIndex(2))->intValue();
        m_pageStartIdx = m_pageStartIdx ? 1 : 0;
        m_prevButton->setVisible(m_pageStartIdx != 0);
        m_nextButton->setVisible(m_pageStartIdx + m_pageEndIdx < m_itemCount);
        updateLevelsLabel();
    }
}

void TopArtistsLayer::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));
        m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
            cocos2d::CCMoveTo::create(0.5f, pos), 0.6f));
        cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
        if (!m_scene) {
            int highestZ = cocos2d::CCScene::getHighestChildZ(runningScene);
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
            m_scene = runningScene;
        } else if (!m_ZOrder) {
            m_ZOrder = 105;
        }
        m_scene->addChild(this);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

void TopArtistsLayer::updateLevelsLabel() {
    int end = m_pageStartIdx + m_pageEndIdx;
    if (end > (int)m_itemCount)
        end = m_itemCount;
    std::string labelText = cocos2d::CCString::createWithFormat(
        "%i to %i of %i", m_pageStartIdx + 1, end, m_itemCount)->getCString();
    m_pageInfoLabel->setString(labelText.c_str());
    m_pageInfoLabel->limitLabelWidth(100.0f, 0.5f, 0.0f);
}

TopArtistsLayer::~TopArtistsLayer() {
}