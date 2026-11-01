#include "RateDemonLayer.h"
#include "RateLevelDelegate.h"
#include "UploadActionPopup.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"
#include "GameManager.h"
#include "GameLevelManager.h"
#include "GameToolbox.h"

RateDemonLayer* RateDemonLayer::create(int levelID)
{
    RateDemonLayer* returnVal = new RateDemonLayer();

    if (returnVal->init(levelID))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool RateDemonLayer::init(int levelID)
{
    if (!FLAlertLayer::init(75))
        return false;

    GameManager* gm = GameManager::sharedState();

    if ((unsigned int)(gm->m_hasRP.random - gm->m_hasRP.seed - 1) > 1)
        m_moderator = gm->m_hasDRP;
    else
        m_moderator = false;

    m_demonRate = -1;
    m_levelID = levelID;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(380.0f, 180.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 90.0f - 20.0f));
    m_mainLayer->addChild(titleLabel);

    m_demons = cocos2d::CCArray::create();
    m_demons->retain();

    cocos2d::CCArray* ratingButtons = cocos2d::CCArray::create();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    for (int i = 0; i < 5; i++)
    {
        cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("difficulty_%02d_btn2_001.png", i);
        cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName->getCString());
        sprite->setScale(1.1f);

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(sprite, nullptr, menu_selector(RateDemonLayer::selectRating));
        m_buttonMenu->addChild(item);
        m_demons->addObject(item);
        item->setTag(i + 1);
        ratingButtons->addObject(item);
    }

    cocos2d::CCPoint ratingPos(winSize.width * 0.5f, winSize.height * 0.5f + 3.0f);
    cocos2d::CCPoint ratingNodePos = m_buttonMenu->convertToNodeSpace(ratingPos);
    GameToolbox::alignItemsHorizontally(ratingButtons, 18.0f, ratingNodePos);
    ratingButtons->removeAllObjects();

    ButtonSprite* cancelSprite = ButtonSprite::create("Cancel", "bigFont.fnt", "GJ_button_06.png");
    CCMenuItemSpriteExtra* cancelBtn = CCMenuItemSpriteExtra::create(cancelSprite, nullptr, menu_selector(RateDemonLayer::onClose));
    m_buttonMenu->addChild(cancelBtn);
    ratingButtons->addObject(cancelBtn);

    ButtonSprite* submitSprite = ButtonSprite::create("Submit", "bigFont.fnt", "GJ_button_01.png");
    CCMenuItemSpriteExtra* submitBtn = CCMenuItemSpriteExtra::create(submitSprite, nullptr, menu_selector(RateDemonLayer::onRate));
    m_submitButton = submitBtn;
    m_buttonMenu->addChild(submitBtn);
    ratingButtons->addObject(submitBtn);

    cocos2d::CCPoint buttonPos(winSize.width * 0.5f, winSize.height * 0.5f - 90.0f + 25.0f);
    cocos2d::CCPoint buttonNodePos = m_buttonMenu->convertToNodeSpace(buttonPos);
    GameToolbox::alignItemsHorizontally(ratingButtons, 15.0f, buttonNodePos);

    selectRating(nullptr);

    return true;
}

void RateDemonLayer::keyBackClicked()
{
    onClose(nullptr);
}

void RateDemonLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);

    if (m_delegate)
        m_delegate->rateLevelClosed();
}

void RateDemonLayer::onClosePopup(UploadActionPopup* popup)
{
    popup->closePopup();
    m_popup = nullptr;

    if (m_uploadFinished)
        onClose(nullptr);
}

void RateDemonLayer::onRate(cocos2d::CCObject* sender)
{
    if (m_demonRate == -1)
        return;

    if (!GameLevelManager::sharedState()->rateDemon(m_levelID, m_demonRate, m_moderator))
    {
        if (!m_moderator)
            onClose(nullptr);
        return;
    }

    if (!m_moderator)
    {
        onClose(nullptr);
        return;
    }

    GameLevelManager::sharedState()->m_uploadActionDelegate = this;

    if (m_popup)
        m_popup->m_delegate = nullptr;

    m_popup = UploadActionPopup::create(this, "Sending rating...");
    m_popup->show();
}

void RateDemonLayer::selectRating(cocos2d::CCObject* sender)
{
    if (sender)
        m_demonRate = sender->getTag();
    else
        m_demonRate = -1;

    for (unsigned int i = 0; i < m_demons->count(); i++)
    {
        CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(m_demons->objectAtIndex(i));
        cocos2d::CCSprite* sprite = static_cast<cocos2d::CCSprite*>(item->getNormalImage());

        if (item->getTag() == m_demonRate)
            sprite->setColor(cocos2d::ccc3(255, 255, 255));
        else
            sprite->setColor(cocos2d::ccc3(150, 150, 150));
    }

    ButtonSprite* submitSprite = static_cast<ButtonSprite*>(m_submitButton->getNormalImage());

    if (m_demonRate == -1)
        submitSprite->setColor(cocos2d::ccc3(150, 150, 150));
    else
        submitSprite->setColor(cocos2d::ccc3(255, 255, 255));
}

void RateDemonLayer::uploadActionFailed(int id, int response)
{
    if (response != 50 || m_levelID != id)
        return;

    if (!m_popup)
        return;

    m_popup->showFailMessage("Failed. Please try again later.");
    m_popup->m_delegate = nullptr;
    m_popup = nullptr;
}

void RateDemonLayer::uploadActionFinished(int id, int response)
{
    if (response != 50 || m_levelID != id)
        return;

    m_uploadFinished = true;

    if (m_popup)
        m_popup->showSuccessMessage("Rating submitted!");
}

RateDemonLayer::~RateDemonLayer()
{
}