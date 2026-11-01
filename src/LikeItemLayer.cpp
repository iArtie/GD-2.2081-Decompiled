#include "LikeItemLayer.h"
#include "FLAlertLayer.h"
#include "GameLevelManager.h"
#include "LikeItemDelegate.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCScale9Sprite.h"

LikeItemLayer* LikeItemLayer::create(LikeItemType type, int id, int parentID)
{
    LikeItemLayer* ret = new LikeItemLayer();
    if (ret->init(type, id, parentID)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LikeItemLayer::init(LikeItemType type, int id, int parentID)
{
    if (!FLAlertLayer::init(75))
        return false;

    m_itemID = id;
    m_commentSourceID = parentID;
    m_itemType = type;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCRect rect = cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", rect);
    bg->setContentSize(cocos2d::CCSize(200.0f, 115.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(LikeItemLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 100.0f + 5.0f, winSize.height * 0.5f + 57.5f - 5.0f)));

    cocos2d::CCSprite* likeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png");
    likeSprite->setScale(1.2f);
    CCMenuItemSpriteExtra* likeBtn = CCMenuItemSpriteExtra::create(
        likeSprite, nullptr, this, menu_selector(LikeItemLayer::onLike));
    m_buttonMenu->addChild(likeBtn);

    cocos2d::CCSprite* dislikeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_dislikeBtn_001.png");
    dislikeSprite->setScale(1.2f);
    CCMenuItemSpriteExtra* dislikeBtn = CCMenuItemSpriteExtra::create(
        dislikeSprite, nullptr, this, menu_selector(LikeItemLayer::onDislike));
    m_buttonMenu->addChild(dislikeBtn);

    likeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 43.0f, winSize.height * 0.5f - 15.0f)));
    dislikeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 43.0f, winSize.height * 0.5f - 15.0f)));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create();
    titleLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 57.5f - 20.0f));
    m_mainLayer->addChild(titleLabel);

    return true;
}

void LikeItemLayer::keyBackClicked()
{
    onClose(nullptr);
}

void LikeItemLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void LikeItemLayer::onDislike(cocos2d::CCObject* sender)
{
    triggerLike(false);
}

void LikeItemLayer::onLike(cocos2d::CCObject* sender)
{
    triggerLike(true);
}

void LikeItemLayer::triggerLike(bool isLiked)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->likeItem(m_itemType, m_itemID, isLiked, m_commentSourceID);
    if (m_likeDelegate)
        m_likeDelegate->likedItem(m_itemType, m_itemID, isLiked);
    onClose(nullptr);
}

LikeItemLayer::~LikeItemLayer()
{
}