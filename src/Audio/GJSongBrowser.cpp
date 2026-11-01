#include "Audio/GJSongBrowser.h"
#include "Layers/GJDropDownLayer.h"
#include "UI/CustomListView.h"
#include "Layers/GJListLayer.h"
#include "Managers/MusicDownloadManager.h"
#include "Layers/FLAlertLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GJSongBrowser* GJSongBrowser::create() {
    auto browser = new GJSongBrowser();
    if (browser && browser->init()) {
        browser->autorelease();
        return browser;
    }
    delete browser;
    return nullptr;
}

bool GJSongBrowser::init() {
    if (!GJDropDownLayer::init("Downloaded Songs")) {
        return false;
    }
    loadPage(0);
    return true;
}

void GJSongBrowser::customSetup() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto menu = cocos2d::CCMenu::create();
    
    auto leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_leftArrow = CCMenuItemSpriteExtra::create(leftSprite, this, menu_selector(GJSongBrowser::onPrevPage));
    menu->addChild(m_leftArrow);
    m_leftArrow->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(
        director->getScreenLeft() + 24.0f,
        centerY
    )));
    
    auto rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightSprite->setFlipX(true);
    m_rightArrow = CCMenuItemSpriteExtra::create(rightSprite, this, menu_selector(GJSongBrowser::onNextPage));
    menu->addChild(m_rightArrow);
    m_rightArrow->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(
        director->getScreenRight() - 24.0f,
        centerY
    )));
    
    auto deleteSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
    auto deleteItem = CCMenuItemSpriteExtra::create(deleteSprite, this, menu_selector(GJSongBrowser::onDeleteAll));
    menu->addChild(deleteItem);
    float rightPos = director->getScreenRight() - (deleteSprite->getContentSize().width * 0.5f + 4.0f);
    deleteItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(
        rightPos,
        director->getScreenBottom() + deleteSprite->getContentSize().height * 0.5f + 4.0f
    )));
    
    m_mainLayer->addChild(menu, 10);
    
    m_countText = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_countText);
    m_countText->setPosition(cocos2d::CCPoint(
        director->getScreenRight() - 7.0f,
        director->getScreenTop() - 4.0f
    ));
    m_countText->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
    m_countText->setScale(0.6f);
    
    auto downloadManager = MusicDownloadManager::sharedState();
    m_downloadedSongs = downloadManager->getDownloadedSongs();
    m_downloadedSongs->retain();
    
    m_listLayer->setOpacity(255);
    m_listLayer->setColor(cocos2d::ccc3(50, 50, 50));
}

void GJSongBrowser::loadPage(int page) {
    m_page = page;
    m_leftArrow->setVisible(page > 0);
    
    auto filteredSongs = cocos2d::CCArray::create();
    auto downloadManager = MusicDownloadManager::sharedState();
    
    for (int i = 0; i < m_downloadedSongs->count(); ++i) {
        auto song = m_downloadedSongs->objectAtIndex(i);
        if (downloadManager->isSongDownloaded(song->m_songID)) {
            filteredSongs->addObject(song);
        }
    }
    
    m_downloadedSongs = filteredSongs;
    m_downloadedSongs->retain();
    
    auto pageSongs = cocos2d::CCArray::create();
    int startIdx = page * 10;
    int endIdx = startIdx + 10;
    
    for (int i = startIdx; i < m_downloadedSongs->count() && i < endIdx; ++i) {
        pageSongs->addObject(m_downloadedSongs->objectAtIndex(i));
    }
    
    m_rightArrow->setVisible(endIdx < m_downloadedSongs->count());
    
    setupPageInfo(m_downloadedSongs->count(), startIdx, 10);
    setupSongBrowser(pageSongs);
}

void GJSongBrowser::setupPageInfo(int itemCount, int pageStartIdx, int pageEndIdx) {
    m_leftArrow->setVisible(pageStartIdx != 0);
    
    int endIdx = pageStartIdx + pageEndIdx;
    if (endIdx > itemCount) {
        endIdx = itemCount;
    }
    
    m_rightArrow->setVisible(endIdx < itemCount);
    
    auto str = cocos2d::CCString::createWithFormat("%i to %i of %i", pageStartIdx + 1, endIdx, itemCount);
    m_countText->setString(str->getCString());
}

void GJSongBrowser::setupSongBrowser(cocos2d::CCArray* songs) {
    m_listLayer->removeChildByTag(9, true);
    
    m_listView = CustomListView::create(songs, BoomListType::SongBrowser, 220.0f, 356.0f);
    m_listView->setTag(9);
    m_listLayer->addChild(m_listView, 6);
}

void GJSongBrowser::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (layer->getTag() == 1 && btn2) {
        auto downloadManager = MusicDownloadManager::sharedState();
        auto songs = downloadManager->getDownloadedSongs();
        
        for (int i = 0; i < songs->count(); ++i) {
            auto song = songs->objectAtIndex(i);
            downloadManager->deleteSong(song->m_songID);
        }
        
        downloadManager->clearUnusedSongs();
        exitLayer(nullptr);
    }
}

void GJSongBrowser::onDeleteAll(cocos2d::CCObject* sender) {
    auto alert = FLAlertLayer::create(
        this,
        "Delete All",
        "Delete all downloaded custom songs?\n",
        "Back",
        "Delete",
        380.0f
    );
    alert->setTag(1);
    alert->show();
}

void GJSongBrowser::onNextPage(cocos2d::CCObject* sender) {
    loadPage(m_page + 1);
}

void GJSongBrowser::onPrevPage(cocos2d::CCObject* sender) {
    loadPage(m_page - 1);
}

void GJSongBrowser::exitLayer(cocos2d::CCObject* sender) {
    auto audio = FMODAudioEngine::sharedEngine();
    audio->stopAllMusic(true);
    GJDropDownLayer::exitLayer(sender);
}

int GJSongBrowser::getSelectedCellIdx() {
    return 0;
}

GJSongBrowser::~GJSongBrowser() {
    if (m_downloadedSongs) {
        m_downloadedSongs->release();
    }
}