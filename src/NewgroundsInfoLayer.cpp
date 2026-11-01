#include "NewgroundsInfoLayer.h"
#include "FLAlertLayer.h"
#include "GameToolbox.h"
#include "TopArtistsLayer.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "cocos2d-ext/CCScale9Sprite.h"

void NewgroundsInfoLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (btn2)
    {
        cocos2d::CCApplication::sharedApplication()->openURL("https:");
    }
}

NewgroundsInfoLayer* NewgroundsInfoLayer::create()
{
    NewgroundsInfoLayer* returnVal = new NewgroundsInfoLayer();
    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool NewgroundsInfoLayer::init()
{
    if (!FLAlertLayer::init(75))
        return false;

    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square02.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 28.0f));
    m_mainLayer->addChild(titleLabel);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(NewgroundsInfoLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    cocos2d::CCPoint closePos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f));
    closeBtn->setPosition(closePos);

    cocos2d::CCLabelBMFont* label1 = cocos2d::CCLabelBMFont::create("", "chatFont.fnt");
    label1->setScale(0.65f);
    label1->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(label1);
    float label1Width = label1->getContentSize().width * label1->getScale();
    label1->setPosition(cocos2d::CCPoint(
        -(label1Width * 0.5f - winSize.width * 0.5f) - 2.0f,
        winSize.height * 0.5f + 70.0f));

    cocos2d::CCLabelBMFont* label2 = cocos2d::CCLabelBMFont::create("", "chatFont.fnt");
    label2->setScale(0.65f);
    label2->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(label2);
    label2->setPosition(cocos2d::CCPoint(
        label1->getPosition().x,
        label1->getPosition().y - 30.0f));

    cocos2d::CCLabelBMFont* label3 = cocos2d::CCLabelBMFont::create("", "chatFont.fnt");
    label3->setScale(0.65f);
    label3->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(label3);
    label3->setPosition(cocos2d::CCPoint(
        label2->getPosition().x,
        label2->getPosition().y - 30.0f));

    cocos2d::CCNode* artistsBtnSprite = ButtonSprite::create(
        "Artists", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_02.png", 25.0f);
    CCMenuItemSpriteExtra* artistsBtn = CCMenuItemSpriteExtra::create(
        artistsBtnSprite, nullptr, menu_selector(NewgroundsInfoLayer::onArtists));
    m_buttonMenu->addChild(artistsBtn);
    artistsBtn->setSizeMult(1.1f);

    cocos2d::CCNode* ngBtnSprite = ButtonSprite::create(
        "Newgrounds", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_01.png", 25.0f);
    CCMenuItemSpriteExtra* ngBtn = CCMenuItemSpriteExtra::create(
        ngBtnSprite, nullptr, menu_selector(NewgroundsInfoLayer::onNewgrounds));
    m_buttonMenu->addChild(ngBtn);
    ngBtn->setSizeMult(1.1f);

    cocos2d::CCNode* guidelinesBtnSprite = ButtonSprite::create(
        "Guidelines", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_05.png", 25.0f);
    CCMenuItemSpriteExtra* guidelinesBtn = CCMenuItemSpriteExtra::create(
        guidelinesBtnSprite, nullptr, menu_selector(NewgroundsInfoLayer::onGuidelines));
    m_buttonMenu->addChild(guidelinesBtn);
    guidelinesBtn->setSizeMult(1.1f);

    cocos2d::CCNode* supporterBtnSprite = ButtonSprite::create(
        "Support NG!", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_03.png", 25.0f);
    CCMenuItemSpriteExtra* supporterBtn = CCMenuItemSpriteExtra::create(
        supporterBtnSprite, nullptr, menu_selector(NewgroundsInfoLayer::onSupporter));
    m_buttonMenu->addChild(supporterBtn);
    supporterBtn->setSizeMult(1.1f);

    cocos2d::CCArray* topButtons = cocos2d::CCArray::create();
    topButtons->addObject(artistsBtn);
    topButtons->addObject(ngBtn);
    topButtons->addObject(guidelinesBtn);
    topButtons->addObject(supporterBtn);

    cocos2d::CCPoint topPos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 145.0f + 40.0f));
    GameToolbox::alignItemsHorisontally(topButtons, 8.0f, topPos);

    cocos2d::CCSprite* changesSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
    changesSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* changesBtn = CCMenuItemSpriteExtra::create(
        changesSprite, nullptr, menu_selector(NewgroundsInfoLayer::onChanges));
    changesBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(changesBtn);
    cocos2d::CCPoint changesPos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 24.0f, winSize.height * 0.5f + 145.0f - 70.0f));
    changesBtn->setPosition(changesPos);

    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_helpBtn2_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
        infoSprite, nullptr, menu_selector(NewgroundsInfoLayer::onInfo));
    infoBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(infoBtn);
    cocos2d::CCPoint infoPos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 27.0f, winSize.height * 0.5f + 145.0f - 27.0f));
    infoBtn->setPosition(infoPos);

    return true;
}

void NewgroundsInfoLayer::keyBackClicked()
{
    onClose(nullptr);
}

void NewgroundsInfoLayer::onArtists(cocos2d::CCObject* sender)
{
    TopArtistsLayer* layer = TopArtistsLayer::create();
    addChild(layer, 100);
}

void NewgroundsInfoLayer::onChanges(cocos2d::CCObject* sender)
{
    std::string text = "Some changes have been made to how <cl>custom music</c> from <cy>Newgrounds.com</c> is handled. Tap the button for more <cg>info</c>.";
    FLAlertLayer* alert = FLAlertLayer::create(this, "Music Changes", text);
    alert->show();
}

void NewgroundsInfoLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void NewgroundsInfoLayer::onGuidelines(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https:");
}

void NewgroundsInfoLayer::onInfo(cocos2d::CCObject* sender)
{
    std::string text = "Each <cg>song</c> has a <cl>unique ID</c> that can be found on <cy>Newgrounds.com</c>. If a song has the address <cl>www.newgrounds.com/audio/listen/</c><cy>568699</c>,\nthen the song ID is <cy>568699</c>.";
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Custom Songs", text);
    alert->show();
}

void NewgroundsInfoLayer::onNewgrounds(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https:");
}

void NewgroundsInfoLayer::onSupporter(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https:");
}

NewgroundsInfoLayer::~NewgroundsInfoLayer()
{
}