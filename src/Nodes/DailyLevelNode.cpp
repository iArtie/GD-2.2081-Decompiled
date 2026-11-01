#include "Nodes/DailyLevelNode.h"
#include "Models/GJGameLevel.h"
#include "Pages/DailyLevelPage.h"
#include "Cells/LevelCell.h"
#include "Sprites/GJChestSprite.h"
#include "Managers/GameStatsManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/FLAlertLayer.h"
#include "Utils/GameToolbox.h"
#include "extension/CCScale9Sprite.h"

void DailyLevelNode::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (btn2)
    {
        m_unkBool = true;
        DailyLevelPage::skipDailyLevel(m_page, this, m_level);
    }
}

DailyLevelNode* DailyLevelNode::create(GJGameLevel* level, DailyLevelPage* page, bool isNew)
{
    DailyLevelNode* pRet = new DailyLevelNode();
    if (pRet && pRet->init(level, page, isNew))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DailyLevelNode::init(GJGameLevel* level, DailyLevelPage* page, bool isNew)
{
    if (!cocos2d::CCNode::init())
        return false;
    
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->registerForcePrio(this, 2);
    
    if (level)
    {
        m_level = level;
        level->retain();
    }
    
    m_page = page;
    m_unkBool = false;
    m_needsDownloading = false;
    m_timeLabel = nullptr;
    m_skipButton = nullptr;
    
    GJTimedLevelType type = page->m_type;
    
    std::string bgTexture;
    std::string crownTexture;
    
    if (type == Weekly)
    {
        bgTexture = "GJ_square05.png";
        crownTexture = "gj_weeklyCrown_001.png";
    }
    else if (type == Event)
    {
        bgTexture = "GJ_square04.png";
        crownTexture = "gj_eventCrown_001.png";
    }
    else
    {
        bgTexture = "GJ_square02.png";
        crownTexture = "gj_dailyCrown_001.png";
    }
    
    bool hasReward = false;
    if (m_level)
        hasReward = (m_level->m_orbCompletion.random - m_level->m_orbCompletion.seed > 99);
    
    float cellHeight = CustomListView::getCellHeight(Level);
    const char* bgTextureUsed = hasReward ? "GJ_square03.png" : bgTexture.c_str();
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        bgTextureUsed, cocos2d::CCRect(0, 0, 80, 80));
    
    if (!hasReward && page->m_type == Weekly)
        bg->setColor(cocos2d::ccColor3B(170, 170, 170));
    
    float height = cellHeight + 26.0f;
    bg->setContentSize(cocos2d::CCSize(380.0f, height));
    bg->setPosition(cocos2d::CCPoint(0, 0));
    addChild(bg, 1);
    
    if (m_level)
    {
        // Level cell
        LevelCell* levelCell = LevelCell::create(360.0f, cellHeight);
        levelCell->m_cellDrawn = true;
        levelCell->loadFromLevel(level);
        addChild(levelCell, 2);
        levelCell->setPosition(cocos2d::CCPoint(-180.0f, -2.0f - (cellHeight * 0.5f)));
        
        // Crown sprite
        cocos2d::CCSprite* crown = cocos2d::CCSprite::createWithSpriteFrameName(crownTexture);
        addChild(crown, 2);
        crown->setPosition(cocos2d::CCPoint(0.0f, height * 0.5f + 12.0f));
        
        if (isNew)
        {
            cocos2d::CCLabelBMFont* newLabel = cocos2d::CCLabelBMFont::create("NEW", "bigFont.fnt");
            newLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
            newLabel->setPosition(cocos2d::CCPoint(182.0f, height * 0.5f - 5.0f));
            newLabel->setScale(0.6f);
            addChild(newLabel, 3);
        }
        
        if (hasReward)
        {
            // Claim reward button
            cocos2d::CCSprite* rewardSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png");
            rewardSprite->setScale(1.0f);
            
            CCMenuItemSpriteExtra* rewardBtn = CCMenuItemSpriteExtra::create(
                rewardSprite, nullptr, this, menu_selector(DailyLevelNode::onClaimReward));
            rewardBtn->setSizeMult(1.1f);
            
            cocos2d::CCMenu* menu = cocos2d::CCMenu::createWithItem(rewardBtn);
            addChild(menu, 10);
            menu->setPosition(cocos2d::CCPoint(150.0f, -2.0f));
            m_unkPoint = menu->getPosition();
            
            cocos2d::CCNode* parent = levelCell->getParent();
            if (parent)
            {
                parent->setScale(0.4f);
                parent->setPosition(parent->getPosition() + cocos2d::CCPoint(-55.0f, 0.0f));
            }
        }
        
        // Daily ID label
        if (m_level)
        {
            int dailyID = m_level->m_dailyID.random - m_level->m_dailyID.seed;
            if (dailyID > 100000)
                dailyID -= 100000;
            if (dailyID > 100000)
                dailyID -= 100000;
            
            cocos2d::CCLabelBMFont* idLabel = cocos2d::CCLabelBMFont::create(
                cocos2d::CCString::createWithFormat("#%i", dailyID)->getCString(), "bigFont.fnt");
            idLabel->setColor(cocos2d::ccColor3B(255, 255, 255));
            idLabel->setOpacity(150);
            idLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
            idLabel->setScale(0.6f);
            addChild(idLabel, 3);
            idLabel->setPosition(cocos2d::CCPoint(183.0f, 7.0f - (height * 0.5f)));
        }
        
        // Skip button
        cocos2d::CCSprite* skipSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
        skipSprite->setScale(0.7f);
        
        CCMenuItemSpriteExtra* skipBtn = CCMenuItemSpriteExtra::create(
            skipSprite, nullptr, this, menu_selector(DailyLevelNode::onSkipLevel));
        m_skipButton = (CCMenuItemSpriteExtra*)cocos2d::CCMenu::createWithItem(skipBtn);
        addChild(m_skipButton, 10);
        m_skipButton->setPosition(cocos2d::CCPoint(-186.0f, height * 0.5f - 4.0f));
        m_skipButton->setVisible(false);
        
        // Reward display
        cocos2d::CCLabelBMFont* rewardLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        addChild(rewardLabel, 10);
        rewardLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        rewardLabel->setScale(0.6f);
        
        if (page->m_type == Weekly || page->m_type == Event)
        {
            GJChestSprite* chest = GJChestSprite::create(3);
            addChild(chest, 10);
            chest->setScale(0.3f);
            chest->setPosition(cocos2d::CCPoint(30.0f, -84.0f));
            rewardLabel->setPosition(chest->getPosition() + cocos2d::CCPoint(-25.0f, 0.0f));
        }
        else
        {
            cocos2d::CCSprite* diamond = cocos2d::CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
            addChild(diamond, 10);
            diamond->setScale(0.8f);
            
            int bonusDiamonds = GameStatsManager::sharedState()->getBonusDiamonds(
                m_level->m_stars.random - m_level->m_stars.seed);
            
            cocos2d::CCLabelBMFont* diamondLabel = cocos2d::CCLabelBMFont::create(
                cocos2d::CCString::createWithFormat("%ix", bonusDiamonds)->getCString(), "bigFont.fnt");
            addChild(diamondLabel, 10);
            diamondLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
            diamondLabel->setScale(0.6f);
            
            float diamondWidth = diamond->getContentSize().width * diamond->getScale();
            float labelWidth = diamondLabel->getContentSize().width * diamondLabel->getScale();
            float totalWidth = diamondWidth + labelWidth;
            float rewardWidth = rewardLabel->getContentSize().width * rewardLabel->getScale();
            float xPos = (totalWidth + rewardWidth + 25.0f) * 0.5f;
            
            diamond->setPosition(cocos2d::CCPoint(xPos - (diamondWidth * 0.5f), -84.0f));
            diamondLabel->setPosition(diamond->getPosition() + cocos2d::CCPoint(-20.0f, 0.0f));
            rewardLabel->setPosition(diamondLabel->getPosition() + 
                cocos2d::CCPoint(-5.0f - (diamondLabel->getContentSize().width * diamondLabel->getScale()), 0.0f));
        }
    }
    else
    {
        bg->setColor(cocos2d::ccColor3B(255, 255, 255));
        bg->setOpacity(75);
        
        m_timeLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        m_timeLabel->setScale(0.5f);
        addChild(m_timeLabel, 3);
        m_needsDownloading = true;
    }
    
    return true;
}

void DailyLevelNode::onClaimReward(cocos2d::CCObject* sender)
{
    if (m_unkBool)
        return;
    
    m_unkBool = true;
    cocos2d::CCPoint pos = cocos2d::CCPoint::operator+(getPosition(), m_unkPoint);
    DailyLevelPage::claimLevelReward(m_page, this, m_level, pos);
    
    if (m_page->m_type != Weekly)
    {
        FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");
    }
    
    sender->setVisible(false);
}

void DailyLevelNode::onSkipLevel(cocos2d::CCObject* sender)
{
    if (m_unkBool)
        return;
    
    std::string levelType;
    GJTimedLevelType type = m_page->m_type;
    
    if (type == Weekly)
        levelType = "weekly demon";
    else if (type == Event)
        levelType = "event level";
    else
        levelType = "daily level";
    
    std::string message = cocos2d::CCString::createWithFormat(
        "There is a <cy>new</c> %s available.\nSkip the current level and load the next?",
        levelType.c_str())->getCString();
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Skip level",
        message,
        "No",
        "Yes"
    );
    alert->show();
}

void DailyLevelNode::showSkipButton()
{
    if (m_skipButton)
    {
        if (!m_level || m_level->m_orbCompletion.random - m_level->m_orbCompletion.seed <= 99)
        {
            m_skipButton->setVisible(true);
            m_skipButton->setScaleY(1.0f);
        }
    }
}

void DailyLevelNode::updateTimeLabel(std::string text)
{
    if (m_timeLabel)
        m_timeLabel->setString(text.c_str());
}

DailyLevelNode::~DailyLevelNode()
{
    CC_SAFE_RELEASE_NULL(m_level);
}