#include "Audio/MusicBrowser.h"
#include "Layers/FLAlertLayer.h"
#include "Managers/MusicDownloadManager.h"
#include "Models/MusicSearchResult.h"
#include "Delegates/MusicBrowserDelegate.h"
#include "Layers/GJCommentListLayer.h"
#include "UI/CustomListView.h"
#include "UI/BoomListView.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"
#include "Sprites/LoadingCircleSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Sprites/ButtonSprite.h"
#include "CCScale9Sprite.h"
#include "Popups/SetTextPopup.h"
#include "Popups/TextAlertPopup.h"
#include "Layers/OptionsScrollLayer.h"
#include "Audio/FMODAudioEngine.h"

void MusicBrowser::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (!layer->getTag() || layer->getTag() == 1) {
        m_searchResult->m_startIndex = 0;
        setupList(m_searchResult);
    }
}

MusicBrowser* MusicBrowser::create(int songID, GJSongType songType)
{
    MusicBrowser* ret = new MusicBrowser();
    if (ret->init(songID, songType)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

int MusicBrowser::getSelectedCellIdx()
{
    return 0;
}

bool MusicBrowser::init(int songID, GJSongType songType)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_songType = songType;
    m_songID = songID;
    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCRect rect(0.0f, 0.0f, 80.0f, 80.0f);
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square02.png", rect);
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_infoLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_infoLabel->setOpacity(240);
    m_infoLabel->setColor(ccc3(255, 255, 255));
    m_infoLabel->setScale(0.5f);
    m_infoLabel->setAnchorPoint(ccp(0.5f, 0.5f));
    m_mainLayer->addChild(m_infoLabel, 10);
    m_infoLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 145.0f + 14.0f));

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(MusicBrowser::onClose));
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setSizeMult(1.6f);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f)));

    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    prevSprite->setScale(0.8f);
    m_prevButton = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, menu_selector(MusicBrowser::onPage));
    m_prevButton->setSizeMult(1.2f);
    m_buttonMenu->addChild(m_prevButton);
    m_prevButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 220.0f + 22.0f, winSize.height * 0.5f - 10.0f)));
    m_prevButton->setTag(0);

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextSprite->setScale(0.8f);
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(MusicBrowser::onPage));
    m_nextButton->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_nextButton);
    m_nextButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 220.0f - 22.0f, winSize.height * 0.5f - 10.0f)));
    m_nextButton->setTag(1);

    cocos2d::CCSprite* refreshSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    refreshSprite->setScale(0.6f);
    m_refreshButton = CCMenuItemSpriteExtra::create(
        refreshSprite, nullptr, this, menu_selector(MusicBrowser::onUpdateLibrary));
    m_buttonMenu->addChild(m_refreshButton);
    m_refreshButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f + 145.0f - 10.0f)));
    m_refreshButton->setVisible(false);

    cocos2d::CCSprite* searchSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
    searchSprite->setScale(1.0f);
    CCMenuItemSpriteExtra* searchBtn = CCMenuItemSpriteExtra::create(
        searchSprite, nullptr, this, menu_selector(MusicBrowser::onSearch));
    searchBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(searchBtn);
    searchBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 220.0f + 24.0f, winSize.height * 0.5f + 75.0f)));

    cocos2d::CCSprite* clearSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_findBtnOff_001.png");
    clearSprite->setScale(1.0f);
    m_clearSearchButton = CCMenuItemSpriteExtra::create(
        clearSprite, nullptr, this, menu_selector(MusicBrowser::onClearSearch));
    m_clearSearchButton->setSizeMult(1.2f);
    m_buttonMenu->addChild(m_clearSearchButton);
    m_clearSearchButton->setPosition(searchBtn->getPosition() + ccp(0.0f, -35.0f));
    m_clearSearchButton->setVisible(false);

    cocos2d::CCSprite* tagIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png");
    ButtonSprite* tagBtn = ButtonSprite::create(
        tagIcon, 0, true, "GJ_button_01.png", 32.0f);
    tagBtn->updateSpriteOffset(ccp(-0.5f, -1.5f));
    m_tagFilterButton = CCMenuItemSpriteExtra::create(
        tagBtn, nullptr, this, menu_selector(MusicBrowser::onTagFilters));
    m_buttonMenu->addChild(m_tagFilterButton);
    m_tagFilterButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 220.0f - 24.0f, winSize.height * 0.5f + 75.0f)));
    m_tagFilterButton->setScale(0.75f);
    m_tagFilterButton->m_baseScale = 0.75f;

    cocos2d::CCSprite* artistIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_musicIcon_001.png");
    ButtonSprite* artistBtn = ButtonSprite::create(
        artistIcon, 0, true, "GJ_button_01.png", 32.0f);
    artistBtn->updateSpriteOffset(ccp(-1.5f, -1.0f));
    m_artistFilterButton = CCMenuItemSpriteExtra::create(
        artistBtn, nullptr, this, menu_selector(MusicBrowser::onArtistFilters));
    m_buttonMenu->addChild(m_artistFilterButton);
    m_artistFilterButton->setPosition(m_tagFilterButton->getPosition() + ccp(0.0f, -35.0f));
    m_artistFilterButton->setScale(0.75f);
    m_artistFilterButton->m_baseScale = 0.75f;

    m_pageLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_pageLabel, 10);
    m_pageLabel->setPosition(ccp(winSize.width * 0.5f + 220.0f - 50.0f, winSize.height * 0.5f + 145.0f - 8.0f));
    m_pageLabel->setAnchorPoint(ccp(1.0f, 1.0f));
    m_pageLabel->setScale(0.4f);

    if (m_songType > GJSongType::Normal) {
        const char* typeFrame = (m_songType == GJSongType::NCS) ? "ncs_med_001.png" : "chompo_med_001.png";
        cocos2d::CCSprite* typeSprite = cocos2d::CCSprite::createWithSpriteFrameName(typeFrame);
        typeSprite->setScale(0.9f);
        m_mainLayer->addChild(typeSprite, 11);
        typeSprite->setPosition(ccp(
            winSize.width * 0.5f + 220.0f - 70.0f,
            winSize.height * 0.5f + 145.0f - 31.0f));
    }

    m_circleSprite = LoadingCircleSprite::create(1.0f);
    m_mainLayer->addChild(m_circleSprite);
    m_circleSprite->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    setupSongControls();

    MusicDownloadManager* mdm = MusicDownloadManager::sharedState();
    mdm->addMusicDownloadDelegate(this);

    if (!MusicDownloadManager::sharedState()->m_musicObjects) {
        mdm = MusicDownloadManager::sharedState();
        mdm->parseMusicLibrary();
    }

    trySetupMusicBrowser();

    this->scheduleUpdate();

    return true;
}

void MusicBrowser::keyBackClicked()
{
    onClose(nullptr);
}

void MusicBrowser::musicActionFailed(GJMusicAction action)
{
    m_circleSprite->hideCircle();

    if (action == GJMusicAction::UpdateMusicLibrary && !m_autoUpdating) {
        TextAlertPopup* popup = TextAlertPopup::create(
            "Music Library is the latest version.", 1.0f, 0.5f, 150, "");
        this->addChild(popup, 100);
    }
}

void MusicBrowser::musicActionFinished(GJMusicAction action)
{
    if (action == GJMusicAction::DownloadOrUpdate && m_gettingURL)
        onUpdateLibrary(nullptr);
    else
        trySetupMusicBrowser();
}

void MusicBrowser::onArtistFilters(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    int maxVisible = (m_songType == GJSongType::NCS) ? 1 : 4;
    OptionsScrollLayer* layer = OptionsScrollLayer::create(
        m_searchResult->m_artistFilterObjects, false, maxVisible);
    layer->setTag(1);
    layer->m_alertProtocol = this;
    layer->show();
}

void MusicBrowser::onClearSearch(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    m_searchResult->m_searchQuery = "";
    m_searchResult->m_startIndex = 0;
    m_searchResult->updateObjects();
    setupList(m_searchResult);
}

void MusicBrowser::onClose(cocos2d::CCObject* sender)
{
    if (m_listLayer) {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }
    if (m_delegate)
        m_delegate->musicBrowserClosed(this);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void MusicBrowser::onPage(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    int totalCount = m_searchResult->m_filterObjects->count();
    int maxPage = (int)ceilf((float)totalCount / 10.0f);
    int currentPage = m_searchResult->m_startIndex;
    int targetPage = (sender->getTag() == 1) ? currentPage + 1 : currentPage - 1;

    if (targetPage >= 0 && targetPage <= maxPage) {
        m_searchResult->m_startIndex = targetPage;
        setupList(m_searchResult);
    }
}

void MusicBrowser::onPlaybackControl(cocos2d::CCObject* sender)
{
    if (!sender->getTag())
        FMODAudioEngine::sharedEngine()->stopMusic();
}

void MusicBrowser::onSearch(cocos2d::CCObject* sender)
{
    SetTextPopup* popup = SetTextPopup::create(
        "", "Search", 20, "Find Song", "OK", 1, 60.0f);
    popup->m_delegate = this;
    popup->m_ZOrder = this->getZOrder() + 1;
    popup->show();
}

void MusicBrowser::onTagFilters(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    OptionsScrollLayer* layer = OptionsScrollLayer::create(
        m_searchResult->m_tagFilterObjects, true, 0);
    layer->setTag(0);
    layer->m_alertProtocol = this;
    layer->show();
}

void MusicBrowser::onUpdateLibrary(cocos2d::CCObject* sender)
{
    m_autoUpdating = (sender == nullptr);

    if (!MusicDownloadManager::sharedState()->m_customContentURL.empty()) {
        m_gettingURL = false;
        MusicDownloadManager::sharedState()->tryUpdateMusicLibrary();
    } else {
        m_gettingURL = true;
        MusicDownloadManager::sharedState()->getCustomContentURL();
    }
}

void MusicBrowser::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void MusicBrowser::setTextPopupClosed(SetTextPopup* popup, std::string text)
{
    if (!m_searchResult)
        return;

    m_searchResult->m_searchQuery = text;
    m_searchResult->m_startIndex = 0;
    m_searchResult->updateObjects();
    setupList(m_searchResult);
}

void MusicBrowser::setupList(MusicSearchResult* musicSearchResult)
{
    if (!musicSearchResult)
        return;

    if (m_libraryVersion > 0 && m_libraryVersion != MusicDownloadManager::sharedState()->m_musicLibraryVersion) {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
            "Music Library Updated to version %i",
            MusicDownloadManager::sharedState()->m_musicLibraryVersion);
        TextAlertPopup* popup = TextAlertPopup::create(str->getCString(), 1.0f, 0.5f, 150, "");
        this->addChild(popup, 100);
    }

    m_libraryVersion = MusicDownloadManager::sharedState()->m_musicLibraryVersion;

    const char* libName = (m_songType == GJSongType::NCS) ? "NCS" : "Music";
    int songCount = (m_songType == GJSongType::NCS)
        ? MusicDownloadManager::sharedState()->m_ncsSongCount
        : MusicDownloadManager::sharedState()->m_musicObjectCount;
    cocos2d::CCString* infoStr = cocos2d::CCString::createWithFormat(
        "%s Library Version %i. Song Count: %i",
        libName, m_libraryVersion, songCount);
    m_infoLabel->setString(infoStr->getCString());

    m_circleSprite->hideCircle();

    if (m_listLayer) {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }

    musicSearchResult->retain();
    if (m_searchResult)
        m_searchResult->release();
    m_searchResult = musicSearchResult;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCArray* visible = cocos2d::CCArray::create();
    int startIdx = 10 * m_searchResult->m_startIndex;
    int endIdx = startIdx + 9;
    int totalCount = m_searchResult->m_filterObjects->count();

    for (int i = startIdx; i <= endIdx && i < totalCount; ++i) {
        cocos2d::CCObject* obj = m_searchResult->m_filterObjects->objectAtIndex(i);
        visible->addObject(obj);
    }

    CustomListView* listView = CustomListView::create(visible, BoomListType::Default, 220.0f, 340.0f);
    cocos2d::ccColor4B color = ccc4(0, 0, 0, 0);
    m_listLayer = GJCommentListLayer::create(listView, "", color, 340.0f, 220.0f, true);
    m_mainLayer->addChild(m_listLayer, 10);
    m_listLayer->setPosition(ccp(
        (winSize.width - 340.0f) * 0.5f,
        (winSize.height - 220.0f) * 0.5f - 10.0f));

    m_refreshButton->setVisible(true);

    int maxPage = (totalCount <= 0) ? 0 : (int)ceilf((float)(totalCount - 1) / 10.0f);
    m_prevButton->setVisible(m_searchResult->m_startIndex > 0);
    m_nextButton->setVisible(m_searchResult->m_startIndex < maxPage);
    m_clearSearchButton->setVisible(!m_searchResult->m_searchQuery.empty());

    ButtonSprite* tagSprite = (ButtonSprite*)m_tagFilterButton->getNormalImage();
    tagSprite->updateBGImage(m_searchResult->m_tagFilter ? "GJ_button_02.png" : "GJ_button_01.png");

    ButtonSprite* artistSprite = (ButtonSprite*)m_artistFilterButton->getNormalImage();
    artistSprite->updateBGImage(m_searchResult->m_artistFilter ? "GJ_button_02.png" : "GJ_button_01.png");

    updatePageLabel();
}

void MusicBrowser::setupMusicBrowser()
{
    if (MusicDownloadManager::sharedState()->m_customContentURL.empty())
        MusicDownloadManager::sharedState()->getCustomContentURL();

    MusicSearchResult* result = MusicSearchResult::create(m_songType);
    AudioSortType sortType = (m_songType == GJSongType::NCS) ? AudioSortType::NCS : AudioSortType::Default;
    result->updateObjects(sortType);
    setupList(result);

    if (!MusicDownloadManager::sharedState()->m_downloadingMusicLibrary)
        onUpdateLibrary(nullptr);
}

void MusicBrowser::setupSongControls()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint basePos(winSize.width * 0.5f - 50.0f, winSize.height * 0.5f + 145.0f - 24.0f);

    m_playSlider = Slider::create(this, menu_selector(MusicBrowser::sliderChanged), 0.7f);
    m_playSlider->m_delegate = this;
    m_mainLayer->addChild(m_playSlider, 20);
    m_playSlider->setPosition(basePos);

    m_playLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_playLabel, 20);
    m_playLabel->setScale(0.6f);
    m_playLabel->setColor(ccc3(255, 255, 255));
    m_playLabel->setOpacity(240);
    m_playLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    m_playLabel->setPosition(basePos + ccp(80.0f, 0.0f));

    cocos2d::CCSprite* stopSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_stopMusicBtn_001.png");
    stopSprite->setScale(0.6f);
    m_playButton = CCMenuItemSpriteExtra::create(
        stopSprite, nullptr, this, menu_selector(MusicBrowser::onPlaybackControl));
    m_buttonMenu->addChild(m_playButton);
    m_playButton->setPosition(m_buttonMenu->convertToNodeSpace(basePos + ccp(-100.0f, 0.0f)));
    m_playButton->setTag(0);
}

void MusicBrowser::sliderChanged(cocos2d::CCObject* sender)
{
    SliderThumb::getValue((SliderThumb*)sender);
}

void MusicBrowser::sliderEnded(Slider* slider)
{
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    if (engine->isMusicPlaying()) {
        SliderThumb* thumb = slider->getThumb();
        float value = SliderThumb::getValue(thumb);
        unsigned int lengthMS = engine->getMusicLengthMS();
        engine->setMusicTimeMS((unsigned int)(value * (float)lengthMS), true);
    }
}

void MusicBrowser::trySetupMusicBrowser()
{
    if (MusicDownloadManager::sharedState()->m_musicObjects) {
        setupMusicBrowser();
    } else if (!MusicDownloadManager::sharedState()->m_customContentURL.empty()) {
        MusicDownloadManager::sharedState()->downloadMusicLibrary();
    } else {
        MusicDownloadManager::sharedState()->getCustomContentURL();
    }
}

void MusicBrowser::update(float dt)
{
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    if (engine->isMusicPlaying()) {
        m_playSlider->enableSlider();
        m_playLabel->setVisible(true);
        m_playButton->setVisible(true);

        unsigned int timeMS = engine->getMusicTimeMS();
        unsigned int lengthMS = engine->getMusicLengthMS();

        cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
            "%i/%is", timeMS / 1000, lengthMS / 1000);
        m_playLabel->setString(str->getCString());

        if (!m_playSlider->getLiveDragging())
            m_playSlider->setValue((float)timeMS / (float)lengthMS);
    } else {
        m_playLabel->setVisible(false);
        m_playSlider->disableSlider();
        m_playButton->setVisible(false);
    }
}

void MusicBrowser::updatePageLabel()
{
    std::string text;
    int count = m_searchResult->m_filterObjects->count();

    if (count <= 0) {
        text = "No matches";
    } else {
        int startIdx = 10 * m_searchResult->m_startIndex;
        int endIdx = startIdx + 9;
        if (count <= endIdx)
            endIdx = count;
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
            "%i to %i of %i", startIdx + 1, endIdx, count);
        text = str->getCString();
    }

    m_pageLabel->setString(text.c_str());
}

MusicBrowser::~MusicBrowser()
{
}