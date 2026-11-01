#include "Layers/CustomSongLayer.h"
#include "UI/CustomSongWidget.h"
#include "Layers/GJDropDownLayer.h"
#include "Audio/GJSongBrowser.h"
#include "Audio/MusicBrowser.h"
#include "Layers/NCSInfoLayer.h"
#include "Layers/NewgroundsInfoLayer.h"
#include "Layers/SongOptionsLayer.h"
#include "Layers/CreateGuidelinesLayer.h"
#include "Models/SongInfoObject.h"
#include "Audio/FMODAudioEngine.h"
#include "Managers/GameManager.h"
#include "extension/CCScale9Sprite.h"

CustomSongLayer* CustomSongLayer::create(CustomSongDelegate* delegate)
{
    CustomSongLayer* pRet = new CustomSongLayer();
    if (pRet && pRet->init(delegate))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void CustomSongLayer::dropDownLayerWillClose(GJDropDownLayer* layer)
{
    if (layer[1].m_uAutoReleaseCount)
    {
        m_songDelegate->songIDChanged(layer[1].m_uReference);
        SongInfoObject* songInfo = SongInfoObject::create(m_songDelegate->getActiveSongID());
        m_songWidget->updateSongObject(songInfo);
    }
}

bool CustomSongLayer::init(CustomSongDelegate* delegate)
{
    incrementForcePrio();
    
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 150)))
        return false;
    
    m_songDelegate = delegate;
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_button_05.png", cocos2d::CCRect(0, 0, 40, 40));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440, 290));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    m_joystickConnected = -1;
    
    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Custom Song", "bigFont.fnt");
    title->setScale(1.0f);
    m_mainLayer->addChild(title);
    title->setPosition(cocos2d::CCPoint(centerX, centerY + 145.0f - 22.0f));
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(CustomSongLayer::onClose));
    closeBtn->setSizeMult(1.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(centerX - 220.0f + 10.0f, centerY + 145.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Song browser button
    cocos2d::CCSprite* savedSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_savedSongsBtn_001.png");
    CCMenuItemSpriteExtra* savedBtn = CCMenuItemSpriteExtra::create(
        savedSprite, nullptr, this, menu_selector(CustomSongLayer::onSongBrowser));
    savedBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(savedBtn);
    savedBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(centerX + 220.0f - 24.0f, centerY - 145.0f + 24.0f + 2.0f)));
    
    // NCS Browser button
    cocos2d::CCSprite* ncsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_ncsLibraryBtn_001.png");
    ncsSprite->setScale(1.2f);
    CCMenuItemSpriteExtra* ncsBtn = CCMenuItemSpriteExtra::create(
        ncsSprite, nullptr, this, menu_selector(CustomSongLayer::onNCSBrowser));
    ncsBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(ncsBtn);
    ncsBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(centerX + 105.0f, centerY + 55.0f)));
    
    // Music Browser button
    cocos2d::CCSprite* musicSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_musicLibraryBtn_001.png");
    musicSprite->setScale(1.2f);
    CCMenuItemSpriteExtra* musicBtn = CCMenuItemSpriteExtra::create(
        musicSprite, nullptr, this, menu_selector(CustomSongLayer::onMusicBrowser));
    musicBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(musicBtn);
    musicBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(centerX, centerY + 55.0f)));
    
    // Newgrounds Browser button
    cocos2d::CCSprite* ngSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_ngBtn_001.png");
    ngSprite->setScale(1.2f);
    CCMenuItemSpriteExtra* ngBtn = CCMenuItemSpriteExtra::create(
        ngSprite, nullptr, this, menu_selector(CustomSongLayer::onNewgroundsBrowser));
    ngBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(ngBtn);
    ngBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(centerX - 105.0f, centerY + 55.0f)));
    
    // Create Guidelines button
    if (m_songDelegate && m_songDelegate->getLevelSettings())
    {
        cocos2d::CCSprite* linesSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_createLinesBtn_001.png");
        CCMenuItemSpriteExtra* linesBtn = CCMenuItemSpriteExtra::create(
            linesSprite, nullptr, this, menu_selector(CustomSongLayer::onCreateLines));
        linesBtn->setSizeMult(1.2f);
        m_buttonMenu->addChild(linesBtn);
        linesBtn->setPosition(m_buttonMenu->convertToNodeSpace(
            cocos2d::CCPoint(centerX - 220.0f + 24.0f, centerY - 145.0f + 24.0f + 2.0f)));
    }
    
    // Options button
    if (m_songDelegate && m_songDelegate->getLevelSettings())
    {
        cocos2d::CCSprite* optionsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
        optionsSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* optionsBtn = CCMenuItemSpriteExtra::create(
            optionsSprite, nullptr, this, menu_selector(CustomSongLayer::onOptions));
        optionsBtn->setSizeMult(2.0f);
        m_buttonMenu->addChild(optionsBtn);
        optionsBtn->setPosition(m_buttonMenu->convertToNodeSpace(
            cocos2d::CCPoint(centerX + 220.0f - 27.0f, centerY + 145.0f - 27.0f)));
    }
    
    // Song ID input
    cocos2d::CCPoint inputPos = cocos2d::CCPoint(centerX - 42.0f, centerY - 20.0f);
    
    // Input background
    cocos2d::extension::CCScale9Sprite* inputBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0, 0, 40, 40));
    inputBg->setOpacity(100);
    inputBg->setContentSize(cocos2d::CCSize(210, 35));
    m_mainLayer->addChild(inputBg, -1);
    inputBg->setPosition(inputPos);
    
    // Input inner background
    cocos2d::extension::CCScale9Sprite* inputInner = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0, 0, 40, 40));
    inputInner->setOpacity(40);
    inputInner->setContentSize(cocos2d::CCSize(203, 29));
    m_mainLayer->addChild(inputInner, 0);
    inputInner->setPosition(inputPos);
    
    m_songIDInput = CCTextInputNode::create(210.0f, 50.0f, "Enter song ID", "Thonburi");
    m_songIDInput->setDelegate(this);
    m_mainLayer->addChild(m_songIDInput, 20);
    m_songIDInput->setPosition(inputPos + cocos2d::CCPoint(-95.0f, 0.0f));
    m_songIDInput->m_textField->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_songIDInput->m_textLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_songIDInput->setMaxLabelScale(0.7f);
    m_songIDInput->setMaxLabelWidth(210.0f);
    m_songIDInput->m_textField->setVisible(false);
    m_songIDInput->m_maxLabelLength = 15;
    m_songIDInput->setAllowedChars("0123456789-");
    m_songIDInput->setLabelPlaceholderColor(cocos2d::ccColor3B(170, 170, 240));
    m_songIDInput->setLabelPlaceholderScale(0.5f);
    m_songIDInput->forceOffset();
    
    // Search button
    ButtonSprite* searchSprite = ButtonSprite::create("Search", 0, 1, 0.8f, true, "goldFont.fnt", "GJ_button_04.png", 35.0f);
    CCMenuItemSpriteExtra* searchBtn = CCMenuItemSpriteExtra::create(
        searchSprite, nullptr, this, menu_selector(CustomSongLayer::onSearch));
    m_buttonMenu->addChild(searchBtn);
    searchBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        inputPos + cocos2d::CCPoint(155.0f, 0.0f)));
    
    // Song widget
    int songID = m_songDelegate->getActiveSongID();
    SongInfoObject* songInfo = nullptr;
    if (songID != 0)
        songInfo = SongInfoObject::create(songID);
    
    m_songWidget = CustomSongWidget::create(
        songInfo,
        m_songDelegate,
        true,   // showPlayButton
        true,   // showDownloadButton
        true,   // showMoreButton
        0,      // bgColor
        0,      // textColor
        false,  // showSize
        false   // isLibrary
    );
    m_mainLayer->addChild(m_songWidget, 2);
    m_songWidget->setPosition(cocos2d::CCPoint(centerX, centerY - 145.0f + 50.0f));
    m_songWidget->getSongInfoIfUnloaded();
    
    return true;
}

void CustomSongLayer::keyBackClicked()
{
    onClose(nullptr);
}

void CustomSongLayer::musicBrowserClosed(MusicBrowser* browser)
{
    if (browser->m_selectedCell)
    {
        m_songDelegate->songIDChanged(browser->m_songID);
        SongInfoObject* songInfo = SongInfoObject::create(m_songDelegate->getActiveSongID());
        m_songWidget->updateSongObject(songInfo);
    }
}

void CustomSongLayer::onClose(cocos2d::CCObject* sender)
{
    if (m_delegate)
        m_delegate->customSongLayerClosed();
    
    m_songIDInput->onClickTrackNode(false);
    FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void CustomSongLayer::onCreateLines(cocos2d::CCObject* sender)
{
    FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    CreateGuidelinesLayer* layer = CreateGuidelinesLayer::create(m_songDelegate, GuidelineCreator);
    layer->show();
}

void CustomSongLayer::onMusicBrowser(cocos2d::CCObject* sender)
{
    int songID = m_songDelegate ? m_songDelegate->getActiveSongID() : 0;
    MusicBrowser* browser = MusicBrowser::create(songID, Music);
    browser->m_delegate = this;
    browser->show();
}

void CustomSongLayer::onNCSBrowser(cocos2d::CCObject* sender)
{
    NCSInfoLayer* layer = NCSInfoLayer::create(this);
    layer->show();
}

void CustomSongLayer::onNewgroundsBrowser(cocos2d::CCObject* sender)
{
    NewgroundsInfoLayer* layer = NewgroundsInfoLayer::create();
    layer->show();
}

void CustomSongLayer::onOptions(cocos2d::CCObject* sender)
{
    SongOptionsLayer* layer = SongOptionsLayer::create(m_songDelegate);
    layer->show();
}

void CustomSongLayer::onSearch(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->getGameVariable("0051"))
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
        m_songIDInput->onClickTrackNode(false);
        
        const char* input = m_songIDInput->getString();
        int songID = atoi(input);
        
        SongInfoObject* songInfo = SongInfoObject::create(songID);
        m_songWidget->updateSongObject(songInfo);
        m_songWidget->getSongInfoIfUnloaded();
    }
    else
    {
        GameManager::sharedState()->setGameVariable("0051", true);
        showNewgroundsMessage();
    }
}

void CustomSongLayer::onSongBrowser(cocos2d::CCObject* sender)
{
    GJSongBrowser* browser = GJSongBrowser::create();
    browser->m_delegate = this;
    
    cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    scene->addChild(browser, scene->getHighestChildZ() + 1);
    browser->showLayer(false);
}

void CustomSongLayer::show()
{
    unsigned char opacity = getOpacity();
    
    m_mainLayer->setScale(0.1f);
    m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
        cocos2d::CCScaleTo::create(0.3f, 1.0f), 1.6f));
    
    if (!m_scene)
        m_scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    if (!m_ZOrder)
        m_ZOrder = 105;
    
    m_scene->addChild(this, m_ZOrder);
    setOpacity(0);
    runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
    setVisible(true);
}

void CustomSongLayer::showNewgroundsMessage()
{
    FLAlertLayer* alert = FLAlertLayer::create(
        nullptr,
        "Newgrounds Info",
        "There are thousands of great custom songs for you to choose from Newgrounds. Please do not upload music you did not create. You are only allowed to use songs from artists that have been scouted on Newgrounds. For more information, please check the <cy>Audio Guidelines</c>."
    );
    alert->show();
}

void CustomSongLayer::textChanged(CCTextInputNode* node)
{
    // Empty implementation
}

void CustomSongLayer::textInputClosed(CCTextInputNode* node)
{
    // Empty implementation
}

void CustomSongLayer::textInputOpened(CCTextInputNode* node)
{
    // Empty implementation
}

void CustomSongLayer::textInputReturn(CCTextInputNode* node)
{
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0, 0)));
}

void CustomSongLayer::textInputShouldOffset(CCTextInputNode* node, float yOffset)
{
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0, yOffset)));
}

CustomSongLayer::~CustomSongLayer()
{
    // Destructor - base class handles cleanup
}