#include "Cells/AchievementCell.h"
#include "UI/GJItemIcon.h"
#include "UI/TextArea.h"
#include "Managers/AchievementManager.h"
#include "Utils/GameToolbox.h"

AchievementCell* AchievementCell::create(const char* identifier, float width, float height)
{
    AchievementCell* cell = new AchievementCell();
    if (cell && cell->init())
    {
        cell->autorelease();
        return cell;
    }
    CC_SAFE_DELETE(cell);
    return nullptr;
}

AchievementCell::AchievementCell(const char* identifier, float width, float height)
{
    TableViewCell::TableViewCell(this, identifier, width, height);
    m_width = width;
    m_height = height;
    init();
}

bool AchievementCell::init()
{
    if (!TableViewCell::init())
        return false;
    
    m_backgroundLayer = cocos2d::CCLayerColor::create();
    m_backgroundLayer->setOpacity(255);
    this->addChild(m_backgroundLayer);
    
    m_mainLayer = cocos2d::CCLayer::create();
    this->addChild(m_mainLayer);
    
    return true;
}

void AchievementCell::draw()
{
    cocos2d::CCSize size(m_width, m_height);
    
    glLineWidth(1.0f);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    
    // Draw top line
    cocos2d::ccDrawLine(ccp(1.0f, size.height), ccp(size.width - 1.0f, size.height));
    
    // Draw bottom line
    cocos2d::ccDrawLine(ccp(1.0f, 0.0f), ccp(size.width - 1.0f, 0.0f));
}

void AchievementCell::loadFromDict(cocos2d::CCDictionary* dict)
{
    m_backgroundLayer->setOpacity(255);
    m_mainLayer->removeAllChildrenWithCleanup(true);
    
    // Get achievement identifier
    std::string identifier = ((cocos2d::CCString*)dict->objectForKey("identifier"))->getCString();
    
    AchievementManager* am = AchievementManager::sharedState();
    bool isEarned = am->isAchievementEarned(identifier.c_str());
    
    GJItemIcon* itemIcon = nullptr;
    float yOffset = 0.0f;
    
    if (!isEarned)
    {
        // Locked achievement - show lock icon
        itemIcon = (GJItemIcon*)cocos2d::CCSprite::createWithSpriteFrameName("playerSquare_001.png");
        cocos2d::CCSprite* lock = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        lock->setPosition(ccp(
            itemIcon->getContentSize().width * 0.5f,
            itemIcon->getContentSize().height * 0.5f
        ));
        itemIcon->addChild(lock);
        itemIcon->setColor(ccc3(50, 50, 50));
    }
    else
    {
        // Unlocked - parse icon from dict
        std::string iconKey = ((cocos2d::CCString*)dict->objectForKey("icon"))->getCString();
        
        if (!iconKey.empty())
        {
            std::vector<std::string> parts = GameToolbox::split(iconKey, '_');
            
            if (parts.size() >= 2)
            {
                std::string typeStr = parts[0];
                int id = atoi(parts[1].c_str());
                UnlockType unlockType = UnlockType::Cube;
                yOffset = 0.0f;
                
                if (typeStr == "ship") unlockType = UnlockType::Ship;
                else if (typeStr == "ball") unlockType = UnlockType::Ball;
                else if (typeStr == "bird") { unlockType = UnlockType::Ufo; yOffset = -5.0f; }
                else if (typeStr == "dart") unlockType = UnlockType::Wave;
                else if (typeStr == "robot") unlockType = UnlockType::Robot;
                else if (typeStr == "spider") unlockType = UnlockType::Spider;
                else if (typeStr == "death") unlockType = UnlockType::Death;
                else if (typeStr == "special") unlockType = UnlockType::Streak;
                else if (typeStr == "swing") unlockType = UnlockType::Swing;
                else if (typeStr == "jetpack") unlockType = UnlockType::Jetpack;
                else if (typeStr == "shipstreak") unlockType = UnlockType::ShipFire;
                else if (typeStr == "color") unlockType = UnlockType::Col1;
                else if (typeStr == "color2") unlockType = UnlockType::Col2;
                
                itemIcon = GJItemIcon::create(
                    unlockType,
                    id,
                    ccc3(255, 255, 255),
                    ccc3(255, 255, 255),
                    false,
                    false,
                    false,
                    ccc3(255, 255, 255)
                );
            }
        }
    }
    
    if (!itemIcon)
    {
        itemIcon = (GJItemIcon*)cocos2d::CCSprite::createWithSpriteFrameName("GJ_achImage_001.png");
    }
    
    itemIcon->setPosition(ccp(35.0f, yOffset + m_height * 0.5f));
    itemIcon->setScale(1.0f);
    m_mainLayer->addChild(itemIcon);
    
    // Container for text
    cocos2d::CCNode* textContainer = cocos2d::CCNode::create();
    textContainer->setPosition(ccp(
        itemIcon->getPosition().x + 32.0f,
        m_height * 0.5f
    ));
    m_mainLayer->addChild(textContainer);
    
    // Title
    std::string title = ((cocos2d::CCString*)dict->objectForKey("title"))->getCString();
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    titleLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    titleLabel->setScale(0.7f);
    
    float titleWidth = titleLabel->getContentSize().width;
    if (titleWidth > 200.0f)
    {
        float newScale = 200.0f / titleWidth;
        if (newScale < 0.7f)
            titleLabel->setScale(newScale);
    }
    textContainer->addChild(titleLabel);
    
    // Description
    std::string descKey = isEarned ? "achievedDescription" : "unachievedDescription";
    std::string desc = ((cocos2d::CCString*)dict->valueForKey(descKey.c_str()))->getCString();
    
    TextArea* descArea = TextArea::create(
        desc.c_str(),
        "chatFont.fnt",
        1.0f,
        190.0f,
        ccp(20.0f, 0.0f),
        0.0f,
        false
    );
    descArea->setAnchorPoint(ccp(0.0f, 0.5f));
    descArea->setPosition(ccp(0.0f, 5.0f));
    descArea->m_height = 16.0f;
    textContainer->addChild(descArea);
    
    // Layout
    float titleHeight = titleLabel->getContentSize().height;
    float descHeight = descArea->getContentSize().height + 17.0f;
    float totalHeight = titleHeight + descHeight;
    
    titleLabel->setPosition(ccp(
        0.0f,
        -(titleHeight * 0.5f - totalHeight * 0.5f) + 3.0f + 2.0f
    ));
    
    descArea->setPosition(ccp(
        0.0f,
        (16.0f - descHeight * 0.5f + totalHeight * 0.5f) - 2.0f
    ));
    
    // Achievement status indicator
    int percent = am->percentForAchievement(identifier.c_str());
    
    if (isEarned)
    {
        cocos2d::CCSprite* completeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
        completeIcon->setAnchorPoint(ccp(0.0f, 0.5f));
        completeIcon->setScale(0.8f);
        completeIcon->setPosition(ccp(m_width - 40.0f, m_height * 0.5f));
        m_mainLayer->addChild(completeIcon);
    }
    else if (percent > 0 && percent < 100)
    {
        cocos2d::CCString* percentStr = cocos2d::CCString::createWithFormat("%i%%", percent);
        cocos2d::CCLabelBMFont* percentLabel = cocos2d::CCLabelBMFont::create(percentStr->getCString(), "bigFont.fnt");
        percentLabel->setScale(0.6f);
        percentLabel->setPosition(ccp(
            itemIcon->getPosition().x,
            itemIcon->getPosition().y - 25.0f
        ));
        m_mainLayer->addChild(percentLabel, 10);
    }
}

void AchievementCell::updateBGColor(int index)
{
    if (index & 1)
        m_backgroundLayer->setColor(ccc3(194, 114, 62));
    else
        m_backgroundLayer->setColor(ccc3(161, 88, 44));
}

AchievementCell::~AchievementCell()
{
    // CCNode handles cleanup
}