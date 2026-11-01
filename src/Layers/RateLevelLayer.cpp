#include "Layers/RateLevelLayer.h"
#include "Delegates/RateLevelDelegate.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Sprites/ButtonSprite.h"
#include "GameLevelManager.h"
#include "Utils/GameToolbox.h"

RateLevelLayer* RateLevelLayer::create(int levelID)
{
    RateLevelLayer* returnVal = new RateLevelLayer();

    if (returnVal->init(levelID))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool RateLevelLayer::init(int levelID)
{
    if (!FLAlertLayer::init(75))
        return false;

    m_levelID = levelID;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(360.0f, 180.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 90.0f - 20.0f));
    m_mainLayer->addChild(titleLabel);

    m_difficulties = cocos2d::CCArray::create();
    m_difficulties->retain();

    cocos2d::CCArray* ratingButtons = cocos2d::CCArray::create();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    for (int i = 0; i < 7; i++)
    {
        std::string frameName;

        if (i == 0)
            frameName = "difficulty_00_btn_001.png";
        else if (i == 1)
            frameName = "difficulty_auto_btn_001.png";
        else
            frameName = cocos2d::CCString::createWithFormat("difficulty_%02d_btn_001.png", i)->getCString();

        cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName.c_str());
        m_difficulties->addObject(sprite);
        sprite->setScale(1.0f);

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(sprite, nullptr, menu_selector(RateLevelLayer::selectRating));
        item->setTag(i);
        m_buttonMenu->addChild(item);
        ratingButtons->addObject(item);
    }

    cocos2d::CCPoint ratingPos(winSize.width * 0.5f, winSize.height * 0.5f);
    cocos2d::CCPoint ratingNodePos = m_buttonMenu->convertToNodeSpace(ratingPos);
    GameToolbox::alignItemsHorizontally(ratingButtons, 16.0f, ratingNodePos);
    ratingButtons->removeAllObjects();

    ButtonSprite* cancelSprite = ButtonSprite::create("Cancel", "bigFont.fnt", "GJ_button_06.png");
    CCMenuItemSpriteExtra* cancelBtn = CCMenuItemSpriteExtra::create(cancelSprite, nullptr, menu_selector(RateLevelLayer::onClose));
    m_buttonMenu->addChild(cancelBtn);
    ratingButtons->addObject(cancelBtn);

    ButtonSprite* submitSprite = ButtonSprite::create("Submit", "bigFont.fnt", "GJ_button_01.png");
    CCMenuItemSpriteExtra* submitBtn = CCMenuItemSpriteExtra::create(submitSprite, nullptr, menu_selector(RateLevelLayer::onRate));
    m_submitButton = submitBtn;
    m_buttonMenu->addChild(submitBtn);
    ratingButtons->addObject(submitBtn);

    cocos2d::CCPoint buttonPos(winSize.width * 0.5f, winSize.height * 0.5f - 90.0f + 25.0f);
    cocos2d::CCPoint buttonNodePos = m_buttonMenu->convertToNodeSpace(buttonPos);
    GameToolbox::alignItemsHorizontally(ratingButtons, 15.0f, buttonNodePos);

    selectRating(nullptr);

    return true;
}

void RateLevelLayer::keyBackClicked()
{
    onClose(nullptr);
}

void RateLevelLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);

    if (m_delegate)
        m_delegate->rateLevelClosed();
}

void RateLevelLayer::onRate(cocos2d::CCObject* sender)
{
    if (m_levelRate != -1)
        onClose(nullptr);
}

void RateLevelLayer::selectRating(cocos2d::CCObject* sender)
{
    if (sender)
        m_levelRate = sender->getTag();
    else
        m_levelRate = -1;

    for (unsigned int i = 0; i < m_difficulties->count(); i++)
    {
        cocos2d::CCSprite* sprite = static_cast<cocos2d::CCSprite*>(m_difficulties->objectAtIndex(i));

        if (m_levelRate == (int)i)
            sprite->setColor(cocos2d::ccc3(255, 255, 255));
        else
            sprite->setColor(cocos2d::ccc3(125, 125, 125));
    }

    ButtonSprite* submitSprite = static_cast<ButtonSprite*>(m_submitButton->getNormalImage());

    if (m_levelRate == -1)
        submitSprite->setColor(cocos2d::ccc3(150, 150, 150));
    else
        submitSprite->setColor(cocos2d::ccc3(255, 255, 255));
}

RateLevelLayer::~RateLevelLayer()
{
}