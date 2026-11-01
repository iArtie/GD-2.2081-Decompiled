#include "Popups/UploadActionPopup.h"
#include "Delegates/UploadActionDelegate.h"
#include "UI/TextArea.h"
#include "UI/LoadingCircle.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

void UploadActionPopup::closePopup()
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

UploadActionPopup* UploadActionPopup::create(UploadPopupDelegate* delegate, std::string str)
{
    UploadActionPopup* popup = new UploadActionPopup();

    if (popup->init(delegate, str))
    {
        popup->autorelease();
        return popup;
    }

    delete popup;
    return nullptr;
}

bool UploadActionPopup::init(UploadPopupDelegate* delegate, std::string str)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    incrementForcePrio();

    m_delegate = delegate;

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square02.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(200.0f, 100.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_textArea = TextArea::create(str, "chatFont.fnt", 1.0f, 400.0f, cocos2d::CCPoint(0.5f, 1.0f), 20.0f, false);
    m_mainLayer->addChild(m_textArea);
    m_textArea->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f));
    m_textArea->setScale(0.8f);

    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->setScale(0.7f);
    m_loadingCircle->show();
    m_loadingCircle->m_sprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 14.0f));

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    m_closeButton = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(UploadActionPopup::onClose));
    m_closeButton->setSizeMult(1.6f);
    m_buttonMenu->addChild(m_closeButton);
    cocos2d::CCPoint closePos(winSize.width * 0.5f - 100.0f + 5.0f, winSize.height * 0.5f + 50.0f - 5.0f);
    m_closeButton->setPosition(m_buttonMenu->convertToNodeSpace(closePos));

    return true;
}

void UploadActionPopup::keyBackClicked()
{
    onClose(nullptr);
}

void UploadActionPopup::onClose(cocos2d::CCObject* sender)
{
    if (m_delegate)
        m_delegate->onClosePopup(this);
    else
        closePopup();
}

void UploadActionPopup::showFailMessage(std::string message)
{
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("exMark_001.png");
    sprite->setPosition(m_loadingCircle->m_sprite->getPosition());
    m_mainLayer->addChild(sprite);

    m_loadingCircle->fadeAndRemove();

    m_textArea->setString(message);

    m_succeeded = false;
}

void UploadActionPopup::showSuccessMessage(std::string message)
{
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    sprite->setScale(1.5f);
    sprite->setPosition(m_loadingCircle->m_sprite->getPosition());
    m_mainLayer->addChild(sprite);

    m_loadingCircle->fadeAndRemove();

    m_textArea->setString(message);

    m_succeeded = true;
}

UploadActionPopup::~UploadActionPopup()
{
}