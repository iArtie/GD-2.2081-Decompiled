#include "Audio/AudioAssetsBrowser.h"
#include "Sprites/LoadingCircleSprite.h"
#include "Models/SongInfoObject.h"
#include "UI/CustomListView.h"
#include "Layers/GJCommentListLayer.h"
#include "Managers/MusicDownloadManager.h"
#include "Utils/GameToolbox.h"
#include "Layers/FLAlertLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Platform/cocos2d.h"
#include <string>

AudioAssetsBrowser* AudioAssetsBrowser::create(std::vector<int>* songIds, std::vector<int>* sfxIds)
{
    AudioAssetsBrowser* pRet = new AudioAssetsBrowser();
    if (pRet && pRet->init(songIds, sfxIds))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

int AudioAssetsBrowser::getCellDelegateType()
{
    return 1;
}

int AudioAssetsBrowser::getSelectedCellIdx()
{
    return 0;
}

bool AudioAssetsBrowser::init(std::vector<int>* songIds, std::vector<int>* sfxIds)
{
    if (!FLAlertLayer::init(this, 150.0f))
        return false;

    m_noElasticity = true;
    m_songIds = *songIds;
    m_sfxIds = *sfxIds;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square02.png",
        cocos2d::CCRect(0, 0, 80, 80)
    );
    bg->setColor(cocos2d::ccColor3B(255, 255, 255));
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Audio Assets", "bigFont.fnt");
    m_mainLayer->addChild(title, 10);
    title->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 20.0f));
    title->setScale(0.8f);

    // Button menu
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    m_buttonMenu = menu;
    m_mainLayer->addChild(menu, 10);

    m_joystickConnected = -1;

    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite,
        nullptr,
        this,
        menu_selector(AudioAssetsBrowser::onClose)
    );
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu->addChild(closeBtn);
    cocos2d::CCPoint closePos = m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f)
    );
    closeBtn->setPosition(closePos);

    // Page indicator
    cocos2d::CCLabelBMFont* pageLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_pageIndicatorLabel = pageLabel;
    m_mainLayer->addChild(pageLabel, 10);
    pageLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 50.0f, winSize.height * 0.5f + 145.0f - 8.0f));
    pageLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
    pageLabel->setScale(0.4f);

    // Info button
    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSprite->setScale(0.65f);
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
        infoSprite,
        nullptr,
        this,
        menu_selector(AudioAssetsBrowser::onInfo)
    );
    infoBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(infoBtn);
    cocos2d::CCPoint infoPos = m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 14.0f, winSize.height * 0.5f + 145.0f - 14.0f - 1.0f)
    );
    infoBtn->setPosition(infoPos);

    // Loading circle
    LoadingCircleSprite* loadingCircle = LoadingCircleSprite::create(1.0f);
    m_loadingCircle = loadingCircle;
    m_mainLayer->addChild(loadingCircle);
    loadingCircle->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    // Register as music download delegate
    MusicDownloadManager::sharedState()->addMusicDownloadDelegate(this);

    setupList();

    return true;
}

void AudioAssetsBrowser::keyBackClicked()
{
    onClose(nullptr);
}

void AudioAssetsBrowser::musicActionFailed(int action)
{
    if (m_loadingCircle)
    {
        m_loadingCircle->hideCircle();
    }
}

void AudioAssetsBrowser::onClose(cocos2d::CCObject* sender)
{
    this->setKeypadEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void AudioAssetsBrowser::onInfo(cocos2d::CCObject* sender)
{
    std::string songList = "";
    std::string missingSongList = "";
    std::string sfxList = "";
    std::string missingSfxList = "";
    
    // Collect song info
    for (int id : m_songIds)
    {
        if (MusicDownloadManager::sharedState()->isSongDownloaded(id))
        {
            if (!songList.empty())
                songList += ", ";
            songList += std::to_string(id);
        }
        else
        {
            if (!missingSongList.empty())
                missingSongList += ", ";
            missingSongList += std::to_string(id);
        }
    }
    
    // Collect SFX info
    for (int id : m_sfxIds)
    {
        if (MusicDownloadManager::sharedState()->isSFXDownloaded(id))
        {
            if (!sfxList.empty())
                sfxList += ", ";
            sfxList += std::to_string(id);
        }
        else
        {
            if (!missingSfxList.empty())
                missingSfxList += ", ";
            missingSfxList += std::to_string(id);
        }
    }
    
    // Build message
    std::string message = "";
    
    if (!songList.empty())
    {
        message += "Songs: <cg>" + songList + "</c>\n";
    }
    
    if (!missingSongList.empty())
    {
        message += "Missing Songs: <cr>" + missingSongList + "</c>\n";
    }
    
    if (!sfxList.empty())
    {
        message += "SFX: <cg>" + sfxList + "</c>\n";
    }
    
    if (!missingSfxList.empty())
    {
        message += "Missing SFX: <cr>" + missingSfxList + "</c>";
    }
    
    if (message.empty())
        message = "No audio assets found.";
    
    FLAlertLayer* alert = FLAlertLayer::create(
        nullptr,
        "Audio Assets",
        message.c_str(),
        "OK",
        nullptr,
        message.length() > 400,
        430.0f,
        false,
        0.0f,
        1.0f
    );
    alert->show();
}

void AudioAssetsBrowser::onPage(cocos2d::CCObject* sender)
{
    // Empty implementation
}

void AudioAssetsBrowser::registerWithTouchDispatcher()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void AudioAssetsBrowser::setupList()
{
    if (m_songInfoObjects)
    {
        m_songInfoObjects->release();
        m_songInfoObjects = nullptr;
    }
    
    if (m_songList)
    {
        // Unregister and remove
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_songList);
        
        m_songList->removeMeAndCleanup();
        m_songList = nullptr;
    }
    
    // Build song info array
    cocos2d::CCArray* songObjects = cocos2d::CCArray::create();
    
    for (int id : m_songIds)
    {
        SongInfoObject* info = MusicDownloadManager::sharedState()->getSongInfoObject(id);
        if (info)
        {
            songObjects->addObject(info);
        }
    }
    
    m_songInfoObjects = songObjects;
    m_songInfoObjects->retain();
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Create list view
    CustomListView* listView = CustomListView::create(
        songObjects,
        BoomListType::AudioAssets,
        220.0f,
        340.0f
    );
    
    cocos2d::ccColor4B bgColor = cocos2d::ccc4(0, 0, 0, 75);
    
    GJCommentListLayer* listLayer = GJCommentListLayer::create(
        listView,
        "",
        bgColor,
        340.0f,
        220.0f,
        true
    );
    
    m_songList = listLayer;
    m_mainLayer->addChild(listLayer, 10);
    
    listLayer->setPosition(cocos2d::CCPoint(
        (winSize.width - 340.0f) * 0.5f,
        (winSize.height - 220.0f) * 0.5f - 10.0f
    ));
    
    updatePageLabel();
}

void AudioAssetsBrowser::trySetupAudioBrowser()
{
    MusicDownloadManager* manager = MusicDownloadManager::sharedState();
    
    if (manager->m_sfxObjects)
    {
        if (manager->m_customContentURL.empty())
        {
            manager->getCustomContentURL();
        }
        setupList();
    }
    else if (manager->m_customContentURL.empty())
    {
        manager->downloadSFXLibrary();
    }
    else
    {
        manager->getCustomContentURL();
    }
}

void AudioAssetsBrowser::updatePageLabel()
{
    // Empty implementation - page label would be updated here
}

AudioAssetsBrowser::~AudioAssetsBrowser()
{
    // Destructor
}