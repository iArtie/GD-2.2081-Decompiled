#include "Pages/WorldLevelPage.h"
#include "Models/GJGameLevel.h"
#include "Nodes/GJWorldNode.h"
#include "Utils/GameToolbox.h"
#include "Layers/SongInfoLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

WorldLevelPage* WorldLevelPage::create(GJGameLevel* level, GJWorldNode* node)
{
    WorldLevelPage* page = new WorldLevelPage();

    if (page->init(level, node))
    {
        page->autorelease();
        return page;
    }

    delete page;
    return nullptr;
}

bool WorldLevelPage::init(GJGameLevel* level, GJWorldNode* node)
{
    incrementForcePrio();

    if (!FLAlertLayer::init(125))
        return false;

    m_worldNode = node;

    if (level)
    {
        m_level = level;
        m_level->retain();
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(320.0f, 260.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(level->m_levelName.c_str(), "bigFont.fnt");
    titleLabel->limitLabelWidth(220.0f, 0.0f, 0.0f);
    m_mainLayer->addChild(titleLabel, 1);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 130.0f - 25.0f));

    cocos2d::CCLabelBMFont* titleShadow = cocos2d::CCLabelBMFont::create(level->m_levelName.c_str(), "bigFont.fnt");
    titleShadow->setScale(titleLabel->getScale());
    m_mainLayer->addChild(titleShadow, 0);
    titleShadow->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0.0f, -2.0f));
    titleShadow->setColor(cocos2d::ccc3(0, 0, 0));
    titleShadow->setOpacity(50);

    for (int corner = 1; corner <= 4; corner++)
    {
        cocos2d::CCSprite* cornerSprite = cocos2d::CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        m_mainLayer->addChild(cornerSprite, 8);

        switch (corner)
        {
            case 1:
                cornerSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 135.0f, winSize.height * 0.5f - 105.0f));
                break;
            case 2:
                cornerSprite->setFlipY(true);
                cornerSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 135.0f, winSize.height * 0.5f + 105.0f));
                break;
            case 3:
                cornerSprite->setFlipX(true);
                cornerSprite->setFlipY(true);
                cornerSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + 135.0f, winSize.height * 0.5f + 105.0f));
                break;
            case 4:
                cornerSprite->setFlipX(true);
                cornerSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + 135.0f, winSize.height * 0.5f - 105.0f));
                break;
        }
    }

    cocos2d::CCSprite* normalBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    normalBar->setColor(cocos2d::ccc3(0, 0, 0));
    normalBar->setOpacity(125);
    normalBar->setScale(0.8f);
    m_mainLayer->addChild(normalBar, 3);
    normalBar->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 48.0f));

    cocos2d::CCSprite* normalFill = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    normalFill->setScaleX(0.992f);
    normalFill->setScaleY(0.86f);
    normalFill->setColor(cocos2d::ccc3(0, 255, 0));
    normalBar->addChild(normalFill, 1);
    normalFill->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    normalFill->setPosition(cocos2d::CCPoint((normalFill->getContentSize().width - normalFill->getContentSize().width * 0.992f) * 0.5f, normalBar->getContentSize().height * 0.5f));

    cocos2d::CCSprite* practiceBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    practiceBar->setColor(cocos2d::ccc3(0, 0, 0));
    practiceBar->setOpacity(125);
    practiceBar->setScale(0.8f);
    m_mainLayer->addChild(practiceBar, 3);
    practiceBar->setPosition(normalBar->getPosition() + cocos2d::CCPoint(0.0f, -45.0f));

    cocos2d::CCSprite* practiceFill = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    practiceFill->setScaleX(0.992f);
    practiceFill->setScaleY(0.86f);
    practiceFill->setColor(cocos2d::ccc3(0, 255, 255));
    practiceBar->addChild(practiceFill, 1);
    practiceFill->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    practiceFill->setPosition(normalFill->getPosition());

    cocos2d::CCLabelBMFont* normalPercentLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(normalPercentLabel, 4);
    normalPercentLabel->setPosition(normalBar->getPosition());
    normalPercentLabel->setScale(0.4f);

    cocos2d::CCLabelBMFont* practicePercentLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(practicePercentLabel, 4);
    practicePercentLabel->setPosition(practiceBar->getPosition());
    practicePercentLabel->setScale(0.4f);

    cocos2d::CCLabelBMFont* normalTitleLabel = cocos2d::CCLabelBMFont::create("Normal", "bigFont.fnt");
    m_mainLayer->addChild(normalTitleLabel, 4);
    normalTitleLabel->setPosition(normalBar->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
    normalTitleLabel->setScale(0.4f);

    cocos2d::CCLabelBMFont* practiceTitleLabel = cocos2d::CCLabelBMFont::create("Practice", "bigFont.fnt");
    m_mainLayer->addChild(practiceTitleLabel, 4);
    practiceTitleLabel->setPosition(practiceBar->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
    practiceTitleLabel->setScale(0.4f);

    float barWidth = normalFill->getContentSize().width;

    int normalPercent = level->getNormalPercent();
    float normalFillWidth = barWidth;
    if (barWidth > (normalPercent / 100.0f) * barWidth)
        normalFillWidth = (level->getNormalPercent() / 100.0f) * barWidth;

    normalFill->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, normalFillWidth, normalFill->getContentSize().height));

    float practiceFillWidth = (level->m_practicePercent / 100.0f) * barWidth;
    if (barWidth <= practiceFillWidth)
        practiceFillWidth = barWidth;

    practiceFill->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, practiceFillWidth, practiceFill->getContentSize().height));

    normalPercentLabel->setString(cocos2d::CCString::createWithFormat("%i%%", normalPercent)->getCString());
    practicePercentLabel->setString(cocos2d::CCString::createWithFormat("%i%%", level->m_practicePercent)->getCString());

    cocos2d::CCArray* starsArray = cocos2d::CCArray::create();

    bool completed = level->getNormalPercent() > 99;
    std::string starFrame = completed ? "GJ_starsIcon_001.png" : "GJ_starsIcon_gray_001.png";

    for (int i = 0; i < level->m_stars; i++)
    {
        cocos2d::CCSprite* starSprite = cocos2d::CCSprite::createWithSpriteFrameName(starFrame.c_str());
        m_mainLayer->addChild(starSprite);
        starSprite->setScale(0.6f);
        starsArray->addObject(starSprite);
        if (!completed)
            starSprite->setOpacity(216);
    }

    GameToolbox::alignItemsHorisontally(starsArray, 15.0f, cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 130.0f + 16.0f), false);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_buttonMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 160.0f + 10.0f, winSize.height * 0.5f + 130.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(WorldLevelPage::onClose));
    closeButton->setSizeMult(1.6f);
    m_buttonMenu->addChild(closeButton);

    cocos2d::CCSprite* playSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    playSprite->setScale(1.0f);
    CCMenuItemSpriteExtra* playButton = CCMenuItemSpriteExtra::create(playSprite, nullptr, menu_selector(WorldLevelPage::onPlay));
    m_buttonMenu->addChild(playButton);
    cocos2d::CCPoint playPos(winSize.width * 0.5f, winSize.height * 0.5f + 32.0f);
    playButton->setPosition(m_buttonMenu->convertToNodeSpace(playPos));

    cocos2d::CCSprite* songSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png");
    songSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* songButton = CCMenuItemSpriteExtra::create(songSprite, nullptr, menu_selector(WorldLevelPage::onSong));
    songButton->setSizeMult(2.0f);
    m_buttonMenu->addChild(songButton);
    cocos2d::CCPoint songPos(winSize.width * 0.5f + 160.0f - 5.0f, winSize.height * 0.5f - 130.0f + 5.0f);
    songButton->setPosition(m_buttonMenu->convertToNodeSpace(songPos));

    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoButton = CCMenuItemSpriteExtra::create(infoSprite, nullptr, menu_selector(WorldLevelPage::onInfo));
    infoButton->setSizeMult(2.0f);
    m_buttonMenu->addChild(infoButton);
    cocos2d::CCPoint infoPos(winSize.width * 0.5f + 160.0f - 5.0f, winSize.height * 0.5f + 130.0f - 5.0f);
    infoButton->setPosition(m_buttonMenu->convertToNodeSpace(infoPos));

    return true;
}

void WorldLevelPage::keyBackClicked()
{
    onClose(nullptr);
}

void WorldLevelPage::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void WorldLevelPage::onInfo(cocos2d::CCObject* sender)
{
    if (m_level)
    {
        std::string levelName = m_level->m_levelName;
        int attempts = m_level->m_attempts;
        int jumps = m_level->m_jumps;
        int normalPercent = m_level->getNormalPercent();

        std::string infoText = cocos2d::CCString::createWithFormat(
            "<cy>%s</c>\n"
            "<cg>Total Attempts</c>: %i\n"
            "<cl>Total Jumps</c>: %i\n"
            "<cp>Normal</c>: %i%%\n"
            "<co>Practice</c>: %i%%",
            levelName.c_str(),
            attempts,
            jumps,
            normalPercent,
            m_level->m_practicePercent)->getCString();

        FLAlertLayer::create(nullptr, "Level Stats", infoText)->show();
    }
}

void WorldLevelPage::onPlay(cocos2d::CCObject* sender)
{
    if (m_worldNode)
        m_worldNode->playStep1();
}

void WorldLevelPage::onSong(cocos2d::CCObject* sender)
{
    if (m_level)
    {
        SongInfoLayer* songInfoLayer = SongInfoLayer::create(m_level->m_audioTrack);
        songInfoLayer->show();
    }
}

void WorldLevelPage::show()
{
    if (!m_noElasticity)
    {
        unsigned __int8 originalOpacity = getOpacity();
        m_mainLayer->setScale(0.1f);
        cocos2d::CCEaseElasticOut* elasticAction = cocos2d::CCEaseElasticOut::create(cocos2d::CCScaleTo::create(0.5f, 1.0f), 0.7f);
        m_mainLayer->runAction(elasticAction);

        cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;

        if (!m_scene)
        {
            int highestZ = cocos2d::CCScene::getHighestChildZ(runningScene);
            int zOrder = 105;
            if (highestZ > 104)
                zOrder = cocos2d::CCScene::getHighestChildZ(runningScene) + 1;
            m_ZOrder = zOrder;
            m_scene = runningScene;
        }

        if (!m_ZOrder)
            m_ZOrder = 105;

        m_scene->addChild(this);
        setOpacity(0);
        cocos2d::CCFadeTo* fadeAction = cocos2d::CCFadeTo::create(0.14f, originalOpacity);
        runAction(fadeAction);
        setVisible(true);
    }
    else
    {
        FLAlertLayer::show();
    }
}

WorldLevelPage::~WorldLevelPage()
{
}