#include "UploadPopup.h"
#include "GJGameLevel.h"
#include "TextArea.h"
#include "LoadingCircle.h"
#include "CCMenuItemSpriteExtra.h"
#include "GameLevelManager.h"
#include "GameManager.h"
#include "LevelBrowserLayer.h"
#include "EditLevelLayer.h"

UploadPopup* UploadPopup::create(GJGameLevel* level)
{
    UploadPopup* popup = new UploadPopup();

    if (popup->init(level))
    {
        popup->autorelease();
        return popup;
    }

    delete popup;
    return nullptr;
}

bool UploadPopup::init(GJGameLevel* level)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    incrementForcePrio();

    m_level = level;
    m_level->retain();

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("square04_001.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setOpacity(255);
    bg->setColor(cocos2d::ccc3(255, 255, 255));
    bg->setContentSize(cocos2d::CCSize(420.0f, 300.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(m_level->m_levelName.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 150.0f - 25.0f));

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_textArea = TextArea::create("Please wait, uploading level...", "chatFont.fnt", 1.0f, 400.0f, cocos2d::CCPoint(0.5f, 1.0f), 20.0f, false);
    m_mainLayer->addChild(m_textArea);
    m_textArea->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 30.0f));

    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->show();
    m_loadingCircle->m_sprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 20.0f));

    if (m_level->m_isUploaded)
    {
        levelUploadFinished(m_level);
        m_textArea->setString("Level already uploaded");
    }
    else
    {
        GameLevelManager::sharedState()->m_levelUploadDelegate = this;
        GameLevelManager::sharedState()->uploadLevel(m_level);
    }

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_backButton = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(UploadPopup::onBack));
    m_buttonMenu->addChild(m_backButton);

    float screenLeft = cocos2d::CCDirector::sharedDirector()->getScreenLeft();
    float screenTop = cocos2d::CCDirector::sharedDirector()->getScreenTop();
    cocos2d::CCPoint backPos(screenLeft + 40.0f, screenTop - 40.0f);
    m_backButton->setPosition(m_buttonMenu->convertToNodeSpace(backPos));

    return true;
}

void UploadPopup::keyBackClicked()
{
}

void UploadPopup::levelUploadFailed(GJGameLevel* level)
{
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("exMark_001.png");
    sprite->setPosition(m_loadingCircle->m_sprite->getPosition());
    m_mainLayer->addChild(sprite);

    m_loadingCircle->fadeAndRemove();

    m_textArea->setString("Upload Failed. Please try again later.");

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCNode* closeSprite = ButtonSprite::create("Close");
    CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(UploadPopup::onClose));
    m_buttonMenu->addChild(closeButton);
    cocos2d::CCPoint closePos(winSize.width * 0.5f, winSize.height * 0.5f - 150.0f + 25.0f);
    closeButton->setPosition(m_buttonMenu->convertToNodeSpace(closePos));

    m_backButton->setVisible(false);
    m_backButton->setEnabled(false);
}

void UploadPopup::levelUploadFinished(GJGameLevel* level)
{
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    sprite->setScale(1.5f);
    sprite->setPosition(m_loadingCircle->m_sprite->getPosition());
    m_mainLayer->addChild(sprite);

    m_loadingCircle->fadeAndRemove();

    m_textArea->setString("Upload Complete");

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCNode* backSprite = ButtonSprite::create("Back");
    CCMenuItemSpriteExtra* backButton = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(UploadPopup::onReturnToLevel));
    m_buttonMenu->addChild(backButton);
    cocos2d::CCPoint backPos(winSize.width * 0.5f, winSize.height * 0.5f - 150.0f + 25.0f);
    backButton->setPosition(m_buttonMenu->convertToNodeSpace(backPos));

    m_backButton->setVisible(false);
    m_backButton->setEnabled(false);
}

void UploadPopup::onBack(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);

    GJSearchObject* searchObject = GJSearchObject::create(SearchType::MyLevels);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObject);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void UploadPopup::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void UploadPopup::onReturnToLevel(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);

    cocos2d::CCScene* scene = EditLevelLayer::scene(m_level);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);

    GameManager* gameManager = GameManager::sharedState();
    if (gameManager->m_playerUserID.random == gameManager->m_playerUserID.seed && !GameManager::sharedState()->m_scoreValid)
    {
        GameLevelManager::sharedState()->updateUserScore();
    }
}

void UploadPopup::show()
{
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    if (!m_scene)
        m_scene = runningScene;
    if (!m_ZOrder)
        m_ZOrder = 105;
    m_scene->addChild(this);
    setVisible(true);
}

UploadPopup::~UploadPopup()
{
}