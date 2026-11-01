#include "Layers/CreatorLayer.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "GameLevelManager.h"
#include "Models/GJChallengeItem.h"
#include "Models/GJSearchObject.h"
#include "Popups/GJPromoPopup.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Pages/DailyLevelPage.h"
#include "Pages/ChallengesPage.h"
#include "Layers/LevelBrowserLayer.h"
#include "Layers/LevelSearchLayer.h"
#include "Layers/LeaderboardsLayer.h"
#include "Layers/GauntletSelectLayer.h"
#include "Layers/SecretLayer2.h"
#include "Layers/SecretRewardsLayer.h"
#include "Layers/GJPathsLayer.h"
#include "Layers/MenuLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCSpriteGrayscale.h"
#include "Cocos/CCSpriteWithHue.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"

bool CreatorLayer::canPlayOnlineLevels()
{
    return true;
}

void CreatorLayer::checkQuestsStatus()
{
    if (m_questsSprite)
    {
        int challengeIndex = 1;
        m_questsSprite->removeAllChildren();
        
        while (true)
        {
            GameStatsManager* statsManager = GameStatsManager::sharedState();
            GJChallengeItem* challenge = statsManager->getChallenge(challengeIndex);
            
            if (challenge && challenge->m_canClaim)
                break;
            
            challengeIndex++;
            if (challengeIndex == 4)
                return;
        }
        
        cocos2d::CCSprite* exMark = cocos2d::CCSprite::createWithSpriteFrameName("exMark_001.png");
        m_questsSprite->addChild(exMark, 10);
        exMark->setScale(0.8f);
        exMark->setPosition(cocos2d::CCPoint(20.0f, m_questsSprite->getScaledContentSize().height));
    }
}

CreatorLayer* CreatorLayer::create()
{
    CreatorLayer* pRet = new CreatorLayer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void CreatorLayer::dialogClosed(DialogLayer* layer)
{
    int tag = layer->getTag(layer);
    if (tag == 0)
    {
        GameManager::sharedState()->setUGV("5", true);
        m_secretDoorSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("secretDoorBtn_open_001.png")
        );
    }
    else if (tag == 1)
    {
        onAdventureMap(nullptr);
    }
}

bool CreatorLayer::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    GameManager::sharedState()->m_currentLevelID = -1;
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    // Background
    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bg, -2);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().height);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().width);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccColor3B(102, 153, 255));

    // Main menu
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    addChild(menu);

    // Button layout calculation
    float scale = 0.8f;
    float spacing = 81.0f;
    float buttonScale = 0.72f;
    
    float availableWidth = winSize.width - 100.0f;
    float totalWidth = (cocos2d::CCSprite::createWithSpriteFrameName("GJ_searchBtn_001.png")->getContentSize().width * 0.8f) + 360.0f;
    float ratio = availableWidth / totalWidth;
    
    if (ratio > 0.9f)
    {
        if (ratio < 1.0f)
        {
            scale = ratio * 0.8f;
            spacing = ratio * 90.0f;
        }
        else
        {
            spacing = 90.0f;
        }
    }
    
    cocos2d::CCPoint menuPos = cocos2d::CCPoint(centerX, centerY);
    menuPos = menu->convertToNodeSpace(menuPos);

    // Create 14 buttons
    for (int i = 0; i < 15; i++)
    {
        std::string spriteName;
        void (CreatorLayer::*handler)(cocos2d::CCObject*);
        bool isQuests = false;
        bool isOnline = false;
        bool useGrayscale = false;

        switch (i)
        {
            case 0:
                spriteName = "GJ_createBtn_001.png";
                handler = &CreatorLayer::onMyLevels;
                break;
            case 1:
                spriteName = "GJ_savedBtn_001.png";
                handler = &CreatorLayer::onSavedLevels;
                break;
            case 2:
                spriteName = "GJ_highscoreBtn_001.png";
                handler = &CreatorLayer::onLeaderboards;
                break;
            case 3:
                spriteName = "GJ_challengeBtn_001.png";
                handler = &CreatorLayer::onChallenge;
                isQuests = true;
                break;
            case 4:
                spriteName = "GJ_versusBtn_001.png";
                handler = &CreatorLayer::onMultiplayer;
                useGrayscale = true;
                break;
            case 5:
                spriteName = "GJ_mapBtn_001.png";
                handler = &CreatorLayer::onAdventureMap;
                useGrayscale = true;
                break;
            case 6:
                spriteName = "GJ_dailyBtn_001.png";
                handler = &CreatorLayer::onDailyLevel;
                break;
            case 7:
                spriteName = "GJ_weeklyBtn_001.png";
                handler = &CreatorLayer::onWeeklyLevel;
                break;
            case 8:
                spriteName = "GJ_eventBtn_001.png";
                handler = &CreatorLayer::onEventLevel;
                break;
            case 9:
                spriteName = "GJ_gauntletsBtn_001.png";
                handler = &CreatorLayer::onGauntlets;
                break;
            case 10:
                spriteName = "GJ_featuredBtn_001.png";
                handler = &CreatorLayer::onFeaturedLevels;
                break;
            case 11:
                spriteName = "GJ_listsBtn_001.png";
                handler = &CreatorLayer::onTopLists;
                break;
            case 12:
                spriteName = "GJ_pathsBtn_001.png";
                handler = &CreatorLayer::onPaths;
                break;
            case 13:
                spriteName = "GJ_mapPacksBtn_001.png";
                handler = &CreatorLayer::onMapPacks;
                break;
            case 14:
                spriteName = "GJ_searchBtn_001.png";
                handler = &CreatorLayer::onOnlineLevels;
                isOnline = true;
                break;
        }

        cocos2d::CCSprite* buttonSprite;
        
        if (useGrayscale)
        {
            CCSpriteGrayscale* grayscale = CCSpriteGrayscale::createWithSpriteFrameName(spriteName);
            grayscale->setEvenLuminance(1.0f);
            grayscale->setColor(cocos2d::ccColor3B(170, 170, 170));
            buttonSprite = grayscale;
        }
        else
        {
            buttonSprite = cocos2d::CCSprite::createWithSpriteFrameName(spriteName);
        }
        
        buttonSprite->setScale(scale);

        CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(
            buttonSprite, nullptr, this, menu_selector(handler)
        );
        menu->addChild(menuItem);

        float x = menuPos.x - (spacing * 0.5f - ((i % 5) - 1.5f) * spacing);
        float y = menuPos.y - (floorf((float)i / 5.0f) - 1.0f) * spacing;
        menuItem->setPosition(cocos2d::CCPoint(x, y));

        // Hide online search button if disabled
        if (isOnline)
        {
            if (GameManager::sharedState()->getGameVariable("0077"))
            {
                menuItem->setEnabled(false);
                menuItem->setVisible(false);
            }
        }

        if (isQuests)
        {
            m_questsSprite = buttonSprite;
            checkQuestsStatus();
        }
    }

    // Side art decorations
    cocos2d::CCSprite* sideArt1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt1->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    sideArt1->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
    ));
    addChild(sideArt1, 1);

    cocos2d::CCSprite* sideArt2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt2->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    sideArt2->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
    ));
    sideArt2->setFlipX(true);
    addChild(sideArt2, 1);

    cocos2d::CCSprite* sideArt3 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt3->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
    sideArt3->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() + 1.0f
    ));
    sideArt3->setFlipY(true);
    addChild(sideArt3, -1);

    cocos2d::CCSprite* sideArt4 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt4->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
    sideArt4->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() + 1.0f
    ));
    sideArt4->setFlipY(true);
    sideArt4->setFlipX(true);
    addChild(sideArt4, -1);

    // Secret Vault button
    bool hasVaultAccess = GameStatsManager::sharedState()->getStat("13") > 49;
    const char* vaultTexture = hasVaultAccess ? "GJ_lock_open_001.png" : "GJ_lockGray_001.png";
    
    cocos2d::CCSprite* vaultSprite = cocos2d::CCSprite::createWithSpriteFrameName(vaultTexture);
    CCMenuItemSpriteExtra* vaultBtn = CCMenuItemSpriteExtra::create(
        vaultSprite, nullptr, this, menu_selector(CreatorLayer::onSecretVault)
    );
    menu->addChild(vaultBtn);
    
    cocos2d::CCPoint vaultPos = menu->convertToNodeSpace(cocos2d::CCPoint(winSize.width - 22.0f, winSize.height - 18.0f));
    vaultBtn->setPosition(vaultPos);
    vaultBtn->setSizeMult(2.0f);

    // Show diamond cost for locked vault
    if (!hasVaultAccess)
    {
        float halfWidth = vaultBtn->getContentSize().width * 0.5f;
        float halfHeight = vaultBtn->getContentSize().height * 0.5f;
        cocos2d::CCPoint labelPos = cocos2d::CCPoint(halfWidth, halfHeight - 22.0f);
        
        cocos2d::CCSprite* diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_diamondsIcon_001.png");
        vaultSprite->addChild(diamondIcon);
        diamondIcon->setPosition(labelPos + cocos2d::CCPoint(9.0f, -1.0f));
        diamondIcon->setScale(0.7f);
        
        cocos2d::CCLabelBMFont* costLabel = cocos2d::CCLabelBMFont::create("50", "bigFont.fnt");
        vaultSprite->addChild(costLabel);
        costLabel->setPosition(labelPos + cocos2d::CCPoint(-8.5f, 0.0f));
        costLabel->setScale(0.4f);
    }

    // Treasure Room button
    bool hasTreasureAccess = GameManager::sharedState()->getUGV("5");
    const char* treasureTexture = hasTreasureAccess ? "secretDoorBtn_open_001.png" : "secretDoorBtn_closed_001.png";
    
    cocos2d::CCSprite* treasureSprite = cocos2d::CCSprite::createWithSpriteFrameName(treasureTexture);
    m_secretDoorSprite = treasureSprite;
    
    CCMenuItemSpriteExtra* treasureBtn = CCMenuItemSpriteExtra::create(
        treasureSprite, nullptr, this, menu_selector(CreatorLayer::onTreasureRoom)
    );
    treasureBtn->m_animationEnabled = false;
    treasureBtn->m_colorEnabled = true;
    menu->addChild(treasureBtn);
    
    cocos2d::CCPoint treasurePos = menu->convertToNodeSpace(cocos2d::CCPoint(winSize.width - 22.0f, 24.0f));
    treasureBtn->setPosition(treasurePos);
    treasureBtn->setSizeMult(1.2f);

    // Back button
    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
        backSprite, nullptr, this, menu_selector(CreatorLayer::onBack)
    );
    backBtn->setSizeMult(1.6f);
    
    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f
    ));

    GameManager::sharedState()->m_currentLevelID = 0;

    if (PlatformToolbox::isControllerConnected())
        GameToolbox::addBackButton(this, backBtn);

    // Update user score if needed
    GameManager* gm = GameManager::sharedState();
    if (gm->m_playerUserID.random == gm->m_playerUserID.seed)
    {
        GameLevelManager::sharedState()->updateUserScore();
    }

    return true;
}

void CreatorLayer::keyBackClicked()
{
    onBack(nullptr);
}

void CreatorLayer::onAdventureMap(cocos2d::CCObject* sender)
{
    GJPromoPopup* popup = GJPromoPopup::create("mapPromo.png");
    popup->show();
}

void CreatorLayer::onBack(cocos2d::CCObject* sender)
{
    cocos2d::CCScene* scene = MenuLayer::scene(false);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    if (cocos2d::CCDirector::sharedDirector()->replaceScene(transition))
        setKeypadEnabled(false);
}

void CreatorLayer::onChallenge(cocos2d::CCObject* sender)
{
    ChallengesPage* page = ChallengesPage::create();
    page->show();
}

void CreatorLayer::onDailyLevel(cocos2d::CCObject* sender)
{
    if (canPlayOnlineLevels())
    {
        DailyLevelPage* page = DailyLevelPage::create(Daily);
        page->show();
    }
}

void CreatorLayer::onEventLevel(cocos2d::CCObject* sender)
{
    if (canPlayOnlineLevels())
    {
        DailyLevelPage* page = DailyLevelPage::create(Event);
        page->show();
    }
}

void CreatorLayer::onFameLevels(cocos2d::CCObject* sender)
{
    GJSearchObject* searchObj = GJSearchObject::create(GJSearchObject::Type::FameLevels);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
    GameManager::sharedState()->m_sceneEnum = 1;
}

void CreatorLayer::onFeaturedLevels(cocos2d::CCObject* sender)
{
    if (canPlayOnlineLevels())
    {
        GJSearchObject* searchObj = GJSearchObject::create(GJSearchObject::Type::FeaturedLevels);
        cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->pushScene(transition);
        GameManager::sharedState()->m_sceneEnum = 1;
    }
}

void CreatorLayer::onGauntlets(cocos2d::CCObject* sender)
{
    cocos2d::CCScene* scene = GauntletSelectLayer::scene(false);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
    GameManager::sharedState()->m_sceneEnum = 1;
}

void CreatorLayer::onLeaderboards(cocos2d::CCObject* sender)
{
    LeaderboardType type = GameLevelManager::sharedState()->m_leaderboardType;
    int stat = GameLevelManager::sharedState()->m_leaderboardStat;
    cocos2d::CCScene* scene = LeaderboardsLayer::scene(type, stat);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void CreatorLayer::onMapPacks(cocos2d::CCObject* sender)
{
    GJSearchObject* searchObj = GJSearchObject::create(GJSearchObject::Type::MapPacks);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
    GameManager::sharedState()->m_sceneEnum = 1;
}

void CreatorLayer::onMultiplayer(cocos2d::CCObject* sender)
{
    std::string speaker = "The Keymaster";
    std::string text = "Versus mode has been <co>delayed</c>,\nbut it's in the works!";
    int duration = 2;
    
    switch (m_versusDialogIndex)
    {
        case 1:
            text = "Yea<d010>.<d010>.<d010>. It's still <cl>delayed</c>.";
            break;
        case 2:
            text = "I blame <cg>RobTop</c>.";
            duration = 10;
            speaker = "Scratch";
            break;
        case 3:
            text = "Who else could we <cr>blame</c>?\nWe're not even real.";
            break;
        case 4:
            text = "Speak for yourself <co>I'm real!</c>";
            speaker = "Scratch";
            duration = 14;
            break;
        case 5:
            text = "<cl>Yea, a real pain in the<d010>.<d010>.<d010>.</c>";
            break;
        case 6:
            text = "You're just <cr>mad</c> because you don't have a <cy>shop</c>.";
            speaker = "Scratch";
            duration = 26;
            break;
        case 7:
            text = "You have a <cy>shop</c>?";
            duration = 4;
            break;
        case 8:
            text = "<co>I HAVE SAID TOO MUCH!\nQUICKLY TO THE <i050><s260>CHOPPER!</s></i></c>";
            speaker = "Scratch";
            duration = 10;
            break;
        case 9:
            text = "You don't have a chop<d010>.<d010>.<d010>.";
            break;
        case 10:
            text = ".<d010>.<d010>.";
            speaker = "Scratch";
            duration = 52;
            break;
        case 11:
            text = ".<d010>.<d010>.<d010>.<d010>.";
            duration = 17;
            break;
        default:
            break;
    }
    
    if (m_versusDialogIndex == 0 || m_versusDialogIndex == 3 || m_versusDialogIndex == 5 || m_versusDialogIndex == 9)
        duration = 2;
    
    DialogObject* dialogObj = DialogObject::create(speaker, text, duration, 1.0f, false, cocos2d::ccColor3B(255, 255, 255));
    DialogLayer* dialog = DialogLayer::create(dialogObj);
    addChild(dialog, 100);
    dialog->updateChatPlacement(Center);
    dialog->animateInRandomSide();
    
    m_versusDialogIndex++;
    if (m_versusDialogIndex > 11)
        m_versusDialogIndex = 0;
}

void CreatorLayer::onMyLevels(cocos2d::CCObject* sender)
{
    GJSearchObject* searchObj;
    
    if (GameManager::sharedState()->m_localSearchType == 102)
    {
        searchObj = GJSearchObject::create(GJSearchObject::Type::LocalLevels);
    }
    else
    {
        searchObj = GJSearchObject::create(GJSearchObject::Type::MyLevels);
        int folder = GameManager::sharedState()->getIntGameVariable("0091");
        if (folder > 998 || folder > 0)
        {
            if (folder > 998)
                folder = 1000;
            searchObj->m_folder = folder;
        }
        else
        {
            searchObj->m_folder = 0;
        }
    }
    
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void CreatorLayer::onOnlineLevels(cocos2d::CCObject* sender)
{
    cocos2d::CCScene* scene = LevelSearchLayer::scene(GameManager::sharedState()->m_levelSearchType);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void CreatorLayer::onOnlyFullVersion(cocos2d::CCObject* sender)
{
    std::string text = "That is only available in the <cl>full</c> version of <cg>Geometry Dash</c>.";
    if (sender->getTag(sender) == 2)
        text = "That is only available in the <cl>full</c> version of <cg>Geometry Dash 2.2</c>.";
    
    DialogObject* dialogObj = DialogObject::create("The Keymaster", text, 2, 1.0f, false, cocos2d::ccColor3B(255, 255, 255));
    DialogLayer* dialog = DialogLayer::create(dialogObj);
    addChild(dialog, 100);
    dialog->updateChatPlacement(Center);
    dialog->animateInRandomSide();
}

void CreatorLayer::onPaths(cocos2d::CCObject* sender)
{
    GJPathsLayer* paths = GJPathsLayer::create();
    paths->show();
}

void CreatorLayer::onSavedLevels(cocos2d::CCObject* sender)
{
    GJSearchObject::Type type;
    if (GameManager::sharedState()->m_savedSearchType == FavouriteLists)
        type = GJSearchObject::Type::FavouriteLists;
    else
        type = GJSearchObject::Type::SavedLevels;
    
    GJSearchObject* searchObj = GJSearchObject::create(type);
    
    int folder = GameManager::sharedState()->getIntGameVariable("0092");
    if (folder <= 998 && folder <= 0)
        folder = 0;
    else if (folder > 998)
        folder = 1000;
    searchObj->m_folder = folder;
    
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void CreatorLayer::onSecretVault(cocos2d::CCObject* sender)
{
    if (GameStatsManager::sharedState()->getStat("13") > 49)
    {
        cocos2d::CCScene* scene = SecretLayer2::scene();
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    }
    else
    {
        std::string text = "zzz";
        
        if (m_vaultDialogIndex < 0)
            m_vaultDialogIndex = (int)(((float)rand() * 4.6566e-10f) * 10.0f);
        else
            m_vaultDialogIndex++;
        
        int index = m_vaultDialogIndex;
        
        switch (index)
        {
            case 0:
                text = "You are not ready...";
                break;
            case 1:
                text = "I can't let you in.";
                break;
            case 2:
                text = "Player used knock.<d040>\nIt's not very effective.";
                break;
            case 3:
                text = "Don't you have something better to do?";
                break;
            case 4:
                text = "That is a bad idea...";
                break;
            case 5:
                text = ".<d030>.<d030>.";
                break;
            case 6:
                text = "I guess you don't like rules...";
                break;
            case 7:
                text = "RubRub told me not to let anyone in.";
                break;
            case 8:
                text = "This is not the door you are looking for.";
                break;
            case 9:
                text = "I don't understand why you keep clicking...";
                break;
            case 10:
                text = "Trust me,<d030> you don't want to come in here.";
                break;
            default:
                m_vaultDialogIndex = 0;
                text = "You are not ready...";
                break;
        }
        
        DialogObject* dialogObj = DialogObject::create("The Keymaster", text, 2, 1.0f, false, cocos2d::ccColor3B(255, 255, 255));
        DialogLayer* dialog = DialogLayer::create(dialogObj);
        addChild(dialog, 100);
        dialog->updateChatPlacement(Center);
        dialog->animateInRandomSide();
    }
}

void CreatorLayer::onTopLists(cocos2d::CCObject* sender)
{
    if (canPlayOnlineLevels())
    {
        GJSearchObject* searchObj = GJSearchObject::create(GJSearchObject::Type::FeaturedLevels);
        searchObj->m_searchMode = 1;
        cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->pushScene(transition);
        GameManager::sharedState()->m_sceneEnum = 1;
    }
}

void CreatorLayer::onTreasureRoom(cocos2d::CCObject* sender)
{
    int keys = GameStatsManager::sharedState()->getStat("21");
    bool hasAccess = GameManager::sharedState()->getUGV("5");
    
    if (hasAccess)
    {
        cocos2d::CCScene* scene = SecretRewardsLayer::scene(false);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    }
    else if (keys > 4)
    {
        cocos2d::CCArray* dialogs = cocos2d::CCArray::create();
        
        DialogObject* d1 = DialogObject::create("The Keymaster", "Well, well, well.<d035>\nLook who it is.", 2, 1.0f, false, cocos2d::ccColor3B(255, 255, 255));
        dialogs->addObject(d1);
        
        DialogObject* d2 = DialogObject::create("The Keymaster", "I see you have the <cg>keys</c>.<d040>\nBut what comes next?", 2, 1.0f, false, cocos2d::ccColor3B(255, 255, 255));
        dialogs->addObject(d2);
        
        DialogObject* d3 = DialogObject::create("The Keymaster", "The <co>door</c> is open.<d040>\nTime to find out<d010>.<d010>.<d010>.", 2, 1.0f, false, cocos2d::ccColor3B(255, 255, 255));
        dialogs->addObject(d3);
        
        DialogLayer* dialog = DialogLayer::createWithObjects(dialogs);
        dialog->m_delegate = this;
        dialog->setTag(0);
        addChild(dialog, 100);
        dialog->updateChatPlacement(Center);
        dialog->animateIn(FromLeft);
    }
    else
    {
        int remaining = 5 - keys;
        std::string text = "Bring me <cy>5</c> <cg>keys</c>,<d020>\nand I will let you pass.";
        
        if (keys > 0)
        {
            std::string suffix = (remaining == 1) ? "" : "s";
            text = cocos2d::CCString::createWithFormat("Collect <cy>%i</c> more <cg>key%s</c>,<d020>\nand I will let you pass.", remaining, suffix.c_str())->getCString();
        }
        
        DialogObject* dialogObj = DialogObject::create("The Keymaster", text, 2, 1.0f, false, cocos2d::ccColor3B(255, 255, 255));
        DialogLayer* dialog = DialogLayer::create(dialogObj);
        addChild(dialog, 100);
        dialog->updateChatPlacement(Center);
        dialog->animateInRandomSide();
    }
}

void CreatorLayer::onWeeklyLevel(cocos2d::CCObject* sender)
{
    if (canPlayOnlineLevels())
    {
        DailyLevelPage* page = DailyLevelPage::create(Weekly);
        page->show();
    }
}

cocos2d::CCScene* CreatorLayer::scene()
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    CreatorLayer* layer = CreatorLayer::create();
    scene->addChild(layer);
    return scene;
}

void CreatorLayer::sceneWillResume()
{
    GameManager* gm = GameManager::sharedState();
    if (gm->m_dailyLevelPage)
        gm->m_dailyLevelPage->refreshDailyPage();
    
    checkQuestsStatus();
}

CreatorLayer::~CreatorLayer()
{
    // Destructor
}