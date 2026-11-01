#include "NCSInfoLayer.h"
#include "CustomSongLayer.h"
#include "CustomSongDelegate.h"
#include "MusicBrowser.h"
#include "MusicBrowserDelegate.h"
#include "FLAlertLayer.h"
#include "GameToolbox.h"
#include "TextArea.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "cocos2d-ext/CCScale9Sprite.h"

NCSInfoLayer* NCSInfoLayer::create(CustomSongLayer* layer)
{
    NCSInfoLayer* returnVal = new NCSInfoLayer();
    if (returnVal->init(layer))
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool NCSInfoLayer::init(CustomSongLayer* layer)
{
    if (!FLAlertLayer::init(75))
        return false;

    m_songLayer = layer;
    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square02.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCSprite* ncsLogo = cocos2d::CCSprite::createWithSpriteFrameName("ncs_large_001.png");
    ncsLogo->setScale(1.2f);
    ncsLogo->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 38.0f));
    m_mainLayer->addChild(ncsLogo);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(NCSInfoLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    cocos2d::CCPoint closePos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f));
    closeBtn->setPosition(closePos);

    std::string infoText = "All NCS catalogue is <cs>copyright free</c>, allowing all content creators to <cs>monetise their content</c> on platforms like YouTube and Twitch!";
    TextArea* textArea = TextArea::create(infoText, "chatFont.fnt", 1.0f, 360.0f,
        cocos2d::CCPoint(0.5f, 1.0f), 22.0f, false);
    m_mainLayer->addChild(textArea);
    textArea->setScale(1.0f);
    textArea->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 35.0f));
    textArea->m_height = 22.0f;

    cocos2d::CCNode* libraryBtnSprite = ButtonSprite::create(
        "Open Library", 0, 0, 0.75f, true, "bigFont.fnt", "GJ_button_01.png", 35.0f);
    CCMenuItemSpriteExtra* libraryBtn = CCMenuItemSpriteExtra::create(
        libraryBtnSprite, nullptr, menu_selector(NCSInfoLayer::onLibrary));
    m_buttonMenu->addChild(libraryBtn);
    libraryBtn->setSizeMult(1.1f);
    cocos2d::CCPoint libraryPos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 50.0f));
    libraryBtn->setPosition(libraryPos);

    cocos2d::CCNode* ncsBtnSprite = ButtonSprite::create(
        "Connect with NCS", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_05.png", 25.0f);
    CCMenuItemSpriteExtra* ncsBtn = CCMenuItemSpriteExtra::create(
        ncsBtnSprite, nullptr, menu_selector(NCSInfoLayer::onNCS));
    m_buttonMenu->addChild(ncsBtn);
    ncsBtn->setSizeMult(1.1f);

    cocos2d::CCNode* ncsIOBtnSprite = ButtonSprite::create(
        "NCS.IO", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* ncsIOBtn = CCMenuItemSpriteExtra::create(
        ncsIOBtnSprite, nullptr, menu_selector(NCSInfoLayer::onNCSIO));
    m_buttonMenu->addChild(ncsIOBtn);
    ncsIOBtn->setSizeMult(1.1f);

    cocos2d::CCNode* usageBtnSprite = ButtonSprite::create(
        "Usage Policy", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* usageBtn = CCMenuItemSpriteExtra::create(
        usageBtnSprite, nullptr, menu_selector(NCSInfoLayer::onNCSUsage));
    m_buttonMenu->addChild(usageBtn);
    usageBtn->setSizeMult(1.1f);

    cocos2d::CCPoint usagePos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 72.0f - 80.0f, winSize.height * 0.5f - 145.0f + 22.0f));
    usageBtn->setPosition(usagePos);

    cocos2d::CCArray* bottomButtons = cocos2d::CCArray::create();
    bottomButtons->addObject(usageBtn);
    bottomButtons->addObject(ncsIOBtn);
    bottomButtons->addObject(ncsBtn);

    cocos2d::CCPoint bottomPos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 145.0f + 22.0f));
    GameToolbox::alignItemsHorisontally(bottomButtons, 14.0f, bottomPos);

    return true;
}

void NCSInfoLayer::keyBackClicked()
{
    onClose(nullptr);
}

void NCSInfoLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void NCSInfoLayer::onLibrary(cocos2d::CCObject* sender)
{
    int activeSongID = 0;
    if (m_songLayer && m_songLayer->m_songDelegate)
        activeSongID = m_songLayer->m_songDelegate->getActiveSongID();

    MusicBrowser* browser = MusicBrowser::create(activeSongID, GJSongType::NCS);
    if (m_songLayer)
        browser->m_delegate = &m_songLayer->MusicBrowserDelegate;
    else
        browser->m_delegate = nullptr;
    browser->show();
}

void NCSInfoLayer::onNCS(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.ncs.io/");
}

void NCSInfoLayer::onNCSIO(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://ncs.io/");
}

void NCSInfoLayer::onNCSUsage(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://ncs.io/usage-policy");
}

NCSInfoLayer::~NCSInfoLayer()
{
}