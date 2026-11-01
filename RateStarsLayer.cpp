#include "RateStarsLayer.h"
#include "RateLevelDelegate.h"
#include "UploadActionPopup.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"
#include "GJDifficultySprite.h"
#include "GameLevelManager.h"
#include "GameToolbox.h"

RateStarsLayer* RateStarsLayer::create(int levelID, bool platformer, bool moderator)
{
    RateStarsLayer* returnVal = new RateStarsLayer();

    if (returnVal->init(levelID, platformer, moderator))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool RateStarsLayer::init(int levelID, bool platformer, bool moderator)
{
    if (!FLAlertLayer::init(75))
        return false;

    m_moderator = moderator;
    m_starsRate = -1;
    m_levelID = levelID;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(380.0f, 180.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    std::string titleText = "Rate ";
    if (m_moderator)
        titleText = "Mod: Suggest ";
    titleText += platformer ? "Moons" : "Stars";

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(titleText.c_str(), "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 90.0f - 20.0f));
    m_mainLayer->addChild(titleLabel);

    m_buttons = cocos2d::CCArray::create();
    m_buttons->retain();

    cocos2d::CCArray* leftRow = cocos2d::CCArray::create();
    cocos2d::CCArray* rightRow = cocos2d::CCArray::create();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    for (int i = 1; i <= 10; i++)
    {
        CCMenuItemSpriteExtra* starsBtn = getStarsButton(i, menu_selector(RateStarsLayer::selectRating), m_buttonMenu, 1.0f);
        m_buttons->addObject(starsBtn);
        starsBtn->setTag(i);

        if (i <= 5)
            leftRow->addObject(starsBtn);
        else
            rightRow->addObject(starsBtn);
    }

    cocos2d::CCPoint leftRowPos(winSize.width * 0.5f - 30.0f, winSize.height * 0.5f + 23.0f + 3.0f);
    cocos2d::CCPoint leftNodePos = m_buttonMenu->convertToNodeSpace(leftRowPos);
    GameToolbox::alignItemsHorizontally(leftRow, 18.0f, leftNodePos);

    cocos2d::CCPoint rightRowPos(winSize.width * 0.5f - 30.0f, winSize.height * 0.5f - 23.0f + 3.0f);
    cocos2d::CCPoint rightNodePos = m_buttonMenu->convertToNodeSpace(rightRowPos);
    GameToolbox::alignItemsHorizontally(rightRow, 18.0f, rightNodePos);

    cocos2d::CCPoint difficultyPos = cocos2d::CCPoint(winSize.width * 0.5f - 30.0f, winSize.height * 0.5f) + cocos2d::CCPoint(172.0f, 2.0f);

    m_difficultySprite = GJDifficultySprite::create(0, GJDifficultyName::Long);
    m_difficultySprite->setScale(1.2f);

    if (m_moderator)
    {
        CCMenuItemSpriteExtra* featureBtn = CCMenuItemSpriteExtra::create(m_difficultySprite, nullptr, menu_selector(RateStarsLayer::onFeature));
        m_buttonMenu->addChild(featureBtn);
        featureBtn->setPosition(m_buttonMenu->convertToNodeSpace(difficultyPos));
    }
    else
    {
        m_mainLayer->addChild(m_difficultySprite, 11);
        m_difficultySprite->setPosition(difficultyPos);
    }

    leftRow->removeAllObjects();

    ButtonSprite* cancelSprite = ButtonSprite::create("Cancel", "bigFont.fnt", "GJ_button_06.png");
    CCMenuItemSpriteExtra* cancelBtn = CCMenuItemSpriteExtra::create(cancelSprite, nullptr, menu_selector(RateStarsLayer::onClose));
    m_buttonMenu->addChild(cancelBtn);
    leftRow->addObject(cancelBtn);

    ButtonSprite* submitSprite = ButtonSprite::create("Submit", "bigFont.fnt", "GJ_button_01.png");
    CCMenuItemSpriteExtra* submitBtn = CCMenuItemSpriteExtra::create(submitSprite, nullptr, menu_selector(RateStarsLayer::onRate));
    m_submitButton = submitBtn;
    m_buttonMenu->addChild(submitBtn);
    leftRow->addObject(submitBtn);

    cocos2d::CCPoint buttonPos(winSize.width * 0.5f, winSize.height * 0.5f - 90.0f + 25.0f);
    cocos2d::CCPoint buttonNodePos = m_buttonMenu->convertToNodeSpace(buttonPos);
    GameToolbox::alignItemsHorizontally(leftRow, 15.0f, buttonNodePos);

    selectRating(nullptr);

    return true;
}

void RateStarsLayer::keyBackClicked()
{
    onClose(nullptr);
}

void RateStarsLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);

    if (m_delegate)
        m_delegate->rateLevelClosed();
}

void RateStarsLayer::onClosePopup(UploadActionPopup* popup)
{
    popup->closePopup();
    m_popup = nullptr;

    if (m_uploadFinished)
        onClose(nullptr);
}

void RateStarsLayer::onFeature(cocos2d::CCObject* sender)
{
    m_featureState = (m_featureState <= 3) ? (m_featureState + 1) : 0;
    m_difficultySprite->updateFeatureState((GJFeatureState)m_featureState);
}

void RateStarsLayer::onRate(cocos2d::CCObject* sender)
{
    if (m_starsRate == -1)
        return;

    if (m_moderator)
    {
        if (GameLevelManager::sharedState()->suggestLevelStars(m_levelID, m_starsRate, m_featureState))
        {
            GameLevelManager::sharedState()->m_uploadActionDelegate = this;

            if (m_popup)
                m_popup->m_delegate = nullptr;

            m_popup = UploadActionPopup::create(this, "Sending rating...");
            m_popup->show();
        }
    }
    else
    {
        onClose(nullptr);
        GameLevelManager::sharedState()->rateStars(m_levelID, m_starsRate);
    }
}

void RateStarsLayer::onToggleCoins(cocos2d::CCObject* sender)
{
    m_coinsToggled = !m_coinsToggled;

    if (m_coinsToggled)
        m_coinSprite->setColor(cocos2d::ccc3(255, 255, 255));
    else
        m_coinSprite->setColor(cocos2d::ccc3(50, 50, 50));
}

void RateStarsLayer::selectRating(cocos2d::CCObject* sender)
{
    if (sender)
        m_starsRate = sender->getTag();
    else
        m_starsRate = -1;

    for (unsigned int i = 0; i < m_buttons->count(); i++)
    {
        CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(m_buttons->objectAtIndex(i));
        ButtonSprite* sprite = static_cast<ButtonSprite*>(item->getNormalImage());

        if (item->getTag() == m_starsRate)
            ButtonSprite::updateBGImage(sprite, "GJ_button_01.png");
        else
            ButtonSprite::updateBGImage(sprite, "GJ_button_04.png");
    }

    ButtonSprite* submitSprite = static_cast<ButtonSprite*>(m_submitButton->getNormalImage());

    if (m_starsRate == -1)
        submitSprite->setColor(cocos2d::ccc3(166, 166, 166));
    else
        submitSprite->setColor(cocos2d::ccc3(255, 255, 255));

    int difficultyFrame;
    if (m_starsRate <= 1)
        difficultyFrame = (m_starsRate == 1) ? -1 : 0;
    else if (m_starsRate == 2)
        difficultyFrame = 1;
    else if (m_starsRate == 3)
        difficultyFrame = 2;
    else if (m_starsRate <= 5)
        difficultyFrame = 3;
    else if (m_starsRate <= 7)
        difficultyFrame = 4;
    else if (m_starsRate <= 9)
        difficultyFrame = 5;
    else if (m_starsRate == 10)
        difficultyFrame = 6;
    else
        difficultyFrame = 0;

    m_difficultySprite->updateDifficultyFrame(difficultyFrame, GJDifficultyName::Short);
}

void RateStarsLayer::uploadActionFailed(int id, int response)
{
    if (m_levelID != id || !m_popup)
        return;

    m_popup->showFailMessage("Failed. Please try again later.");
    m_popup->m_delegate = nullptr;
    m_popup = nullptr;
}

void RateStarsLayer::uploadActionFinished(int id, int response)
{
    if (m_levelID != id)
        return;

    m_uploadFinished = true;

    if (m_popup)
        m_popup->showSuccessMessage("Rating submitted!");
}

RateStarsLayer::~RateStarsLayer()
{
}