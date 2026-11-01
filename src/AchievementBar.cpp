#include "AchievementBar.h"
#include "AchievementNotifier.h"
#include "GameToolbox.h"
#include "GJItemIcon.h"

AchievementBar* AchievementBar::create(const char* title, const char* desc, const char* icon, bool quest)
{
    AchievementBar* bar = new AchievementBar();
    if (bar && bar->init(title, desc, icon, quest))
    {
        bar->autorelease();
        return bar;
    }
    CC_SAFE_DELETE(bar);
    return nullptr;
}

bool AchievementBar::init(const char* title, const char* desc, const char* icon, bool quest)
{
    if (!CCNodeRGBA::init())
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = ccp(winSize.width * 0.5f, 40.0f);

    // Background layer
    m_layerColor = cocos2d::CCLayerColor::create(ccc4(0, 0, 0, 0));
    m_layerColor->setPosition(center);
    this->addChild(m_layerColor);

    // Main background
    const char* bgName = quest ? "GJ_square02.png" : "GJ_square01.png";
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        bgName, cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(300.0f, 80.0f));
    m_layerColor->addChild(bg, 1);
    m_bg = bg;

    // Glow sprite
    cocos2d::CCSprite* glow = cocos2d::CCSprite::createWithSpriteFrameName("achievementGlow_001.png");
    glow->setScale(4.0f);
    glow->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
    m_layerColor->addChild(glow, -1);
    m_achievementGlowSprite = glow;

    // Border overlay
    cocos2d::extension::CCScale9Sprite* border = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", cocos2d::CCRect(0, 0, 80, 80));
    border->setContentSize(cocos2d::CCSize(300.0f, 80.0f));
    border->setPosition(bg->getPosition());
    border->setOpacity(0);
    border->setBlendAdditive(true);
    m_layerColor->addChild(border, 100);
    m_bg2 = border;

    // Icon position
    cocos2d::CCPoint iconPos = ccp(40.0f, 40.0f);

    GJItemIcon* itemIcon = nullptr;

    if (quest)
    {
        // Quest icon
        itemIcon = (GJItemIcon*)cocos2d::CCSprite::createWithSpriteFrameName(icon);
        if (itemIcon)
        {
            float iconScale = 30.0f / itemIcon->getContentSize().width;
            itemIcon->setScale(iconScale);
        }
    }
    else if (icon)
    {
        // Parse icon string like "cube_1", "ship_5", "ball_3", etc.
        std::string iconStr = icon;
        std::vector<std::string> parts = GameToolbox::split(iconStr, '_');

        if (parts.size() >= 2)
        {
            std::string typeStr = parts[0];
            int id = atoi(parts[1].c_str());
            UnlockType unlockType = UnlockType::Cube;
            const char* typeName = "Icon";
            float yOffset = 0.0f;

            if (typeStr == "ship")
            {
                unlockType = UnlockType::Ship;
                typeName = "Ship";
            }
            else if (typeStr == "ball")
            {
                unlockType = UnlockType::Ball;
                typeName = "Ball";
            }
            else if (typeStr == "bird")
            {
                unlockType = UnlockType::Ufo;
                typeName = "UFO";
                yOffset = -8.0f;
            }
            else if (typeStr == "dart")
            {
                unlockType = UnlockType::Wave;
                typeName = "Wave";
            }
            else if (typeStr == "robot")
            {
                unlockType = UnlockType::Robot;
                typeName = "Robot";
            }
            else if (typeStr == "spider")
            {
                unlockType = UnlockType::Spider;
                typeName = "Spider";
            }
            else if (typeStr == "death")
            {
                unlockType = UnlockType::Death;
                typeName = "Death Effect";
            }
            else if (typeStr == "special")
            {
                unlockType = UnlockType::Streak;
                typeName = "Trail";
            }
            else if (typeStr == "color")
            {
                unlockType = UnlockType::Col1;
                typeName = "Main Color";
            }
            else if (typeStr == "color2")
            {
                unlockType = UnlockType::Col2;
                typeName = "Secondary Color";
            }
            else if (typeStr == "swing")
            {
                unlockType = UnlockType::Swing;
                typeName = "Swing";
            }
            else if (typeStr == "jetpack")
            {
                unlockType = UnlockType::Jetpack;
                typeName = "Jetpack";
            }
            else if (typeStr == "shipfire")
            {
                unlockType = UnlockType::ShipFire;
                typeName = "Ship Fire";
            }

            std::string unlockMsg = "Unlocked new " + std::string(typeName) + "!";
            itemIcon = GJItemIcon::create(
                unlockType,
                id,
                ccc3(255, 255, 255),
                ccc3(255, 255, 255),
                false,
                false,
                true,
                ccc3(255, 255, 255)
            );

            if (itemIcon)
                desc = unlockMsg.c_str();
        }
    }

    if (!itemIcon)
    {
        itemIcon = (GJItemIcon*)cocos2d::CCSprite::createWithSpriteFrameName("GJ_achImage_001.png");
    }

    itemIcon->setPosition(iconPos);
    m_layerColor->addChild(itemIcon, 3);
    m_achievementSprite = itemIcon;

    // Container for title and description
    cocos2d::CCNode* textContainer = cocos2d::CCNode::create();
    m_layerColor->addChild(textContainer, 2);

    // Title label
    float scale = quest ? 0.8f : 0.7f;
    const char* fontName = quest ? "goldFont.fnt" : "bigFont.fnt";
    
    m_titleLabel = cocos2d::CCLabelBMFont::create(title, fontName);
    m_titleLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    m_titleLabel->setScale(scale);
    
    float titleWidth = m_titleLabel->getContentSize().width;
    if (titleWidth > 200.0f)
    {
        float newScale = 200.0f / titleWidth;
        if (newScale < scale)
            m_titleLabel->setScale(newScale);
    }
    
    textContainer->addChild(m_titleLabel);

    // Description text area
    const char* descText = desc;
    m_achievementDescription = TextArea::create(
        descText,
        "chatFont.fnt",
        1.0f,
        200.0f,
        ccp(20.0f, 0.0f),
        0.0f,
        false
    );
    m_achievementDescription->setAnchorPoint(ccp(0.0f, 0.5f));
    m_achievementDescription->setPosition(ccp(0.0f, 5.0f));
    m_achievementDescription->m_height = 16.0f;
    textContainer->addChild(m_achievementDescription);

    // Layout
    float titleHeight = m_titleLabel->getContentSize().height;
    float descHeight = m_achievementDescription->getContentSize().height + 17.0f;
    float totalHeight = titleHeight + descHeight;
    
    m_titleLabel->setPosition(ccp(
        0.0f,
        -(titleHeight * 0.5f - totalHeight * 0.5f) + 2.0f + (quest ? 2.5f : 0.0f)
    ));
    
    m_achievementDescription->setPosition(ccp(
        1.0f,
        (15.0f - descHeight * 0.5f + totalHeight * 0.5f) - 1.0f
    ));

    // Center the container based on icon position
    float iconWidth = m_achievementSprite->getContentSize().width * m_achievementSprite->getScale();
    float titleWidth2 = m_titleLabel->getContentSize().width * m_titleLabel->getScale();
    float maxWidth = m_achievementDescription->m_maxWidth;
    
    float textWidth = (titleWidth2 >= maxWidth) ? (titleWidth2 + 5.0f) : (maxWidth + 5.0f);
    float totalWidth = textWidth + iconWidth * 0.5f + 30.0f;
    
    m_achievementSprite->setPosition(ccp(
        -(totalWidth * 0.5f - iconWidth * 0.5f),
        0.0f
    ));
    
    textContainer->setPosition(ccp(
        m_achievementSprite->getPosition().x + 30.0f,
        0.0f
    ));

    m_unkUnused = 0x42800000; // 64.0f float constant

    return true;
}

void AchievementBar::show()
{
    cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    if (!m_nextScene)
        m_nextScene = scene;

    int highestZ = cocos2d::CCScene::getHighestChildZ(m_nextScene);
    int zOrder = (highestZ > 999) ? (highestZ + 1) : 1000;
    m_nextScene->addChild(this, zOrder);

    // Scale animation
    m_layerColor->setScale(1.2f);
    cocos2d::CCActionInterval* scaleIn = cocos2d::CCScaleTo::create(0.4f, 1.0f);
    cocos2d::CCActionInterval* easeIn = cocos2d::CCEaseInOut::create(scaleIn, 2.0f);
    m_layerColor->runAction(easeIn);

    // Position at bottom of screen
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_layerColor->setPosition(ccp(winSize.width * 0.5f, winSize.height - 40.0f - 8.0f));

    // Auto-hide timer
    cocos2d::CCAction* wait = cocos2d::CCDelayTime::create(3.2f);
    cocos2d::CCAction* callback = cocos2d::CCCallFunc::create(
        AchievementNotifier::sharedState(),
        callfunc_selector(AchievementNotifier::achievementDisplayFinished)
    );
    cocos2d::CCAction* sequence = cocos2d::CCSequence::create(wait, callback, nullptr);
    m_layerColor->runAction(sequence);

    // Fade out whole bar
    cocos2d::CCAction* wait2 = cocos2d::CCDelayTime::create(2.4f);
    cocos2d::CCAction* fadeOut = cocos2d::CCFadeOut::create(0.8f);
    cocos2d::CCAction* fadeSequence = cocos2d::CCSequence::create(wait2, cocos2d::CCEaseIn::create(fadeOut, 2.0f), nullptr);
    this->runAction(fadeSequence);

    // Border flash
    m_bg2->setOpacity(255);
    cocos2d::CCAction* borderFade = cocos2d::CCEaseInOut::create(
        cocos2d::CCFadeOut::create(0.9f), 1.8f
    );
    m_bg2->runAction(borderFade);

    // Glow animation
    m_achievementGlowSprite->setOpacity(0);
    cocos2d::CCAction* glowFadeIn = cocos2d::CCEaseInOut::create(
        cocos2d::CCFadeIn::create(0.35f), 2.0f
    );
    cocos2d::CCAction* glowDelay = cocos2d::CCDelayTime::create(0.5f);
    cocos2d::CCAction* glowFadeOut = cocos2d::CCEaseInOut::create(
        cocos2d::CCFadeOut::create(2.4f), 2.0f
    );
    cocos2d::CCAction* glowSequence = cocos2d::CCSequence::create(
        glowFadeIn, glowDelay, glowFadeOut, nullptr
    );
    m_achievementGlowSprite->runAction(glowSequence);
}

void AchievementBar::setOpacity(GLubyte opacity)
{
    m_bg->setOpacity(opacity);
    m_achievementSprite->setOpacity(opacity);
    m_titleLabel->setOpacity(opacity);
    m_achievementDescription->setOpacity(opacity);
}

AchievementBar::~AchievementBar()
{
    // CCNode handles cleanup
}