#include "Layers/AchievementsLayer.h"
#include "Managers/AchievementManager.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"

AchievementsLayer* AchievementsLayer::create()
{
    AchievementsLayer* layer = new AchievementsLayer();
    if (layer && GJDropDownLayer::init(layer, "Achievements"))
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void AchievementsLayer::customSetup()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    bool isControllerConnected = PlatformToolbox::isControllerConnected();

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();

    float centerY = winSize.height * 0.5f;

    // Previous page button
    const char* leftArrowName = isControllerConnected ? "controllerBtn_DPad_Left_001.png" : "GJ_arrow_01_001.png";
    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName(leftArrowName);

    m_prevPageButton = CCMenuItemSpriteExtra::create(
        leftArrow,
        nullptr,
        this,
        menu_selector(AchievementsLayer::onPrevPage)
    );

    menu->addChild(m_prevPageButton);
    float screenLeft = cocos2d::CCDirector::getScreenLeft(cocos2d::CCDirector::sharedDirector());
    cocos2d::CCPoint leftPos = menu->convertToNodeSpace(ccp(screenLeft + 24.0f, centerY));
    m_prevPageButton->setPosition(leftPos);

    // Next page button
    cocos2d::CCSprite* rightArrow = nullptr;
    if (isControllerConnected)
    {
        rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_DPad_Right_001.png");
    }
    else
    {
        rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        rightArrow->setFlipX(true);
    }

    m_nextPageButton = CCMenuItemSpriteExtra::create(
        rightArrow,
        nullptr,
        this,
        menu_selector(AchievementsLayer::onNextPage)
    );

    menu->addChild(m_nextPageButton);
    float screenRight = cocos2d::CCDirector::getScreenRight(cocos2d::CCDirector::sharedDirector());
    cocos2d::CCPoint rightPos = menu->convertToNodeSpace(ccp(screenRight - 24.0f, centerY));
    m_nextPageButton->setPosition(rightPos);

    m_mainLayer->addChild(menu, 10);

    // Page label
    m_pageLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_pageLabel->setAnchorPoint(ccp(1.0f, 1.0f));
    m_pageLabel->setScale(0.6f);
    m_pageLabel->setPosition(ccp(
        cocos2d::CCDirector::getScreenRight(cocos2d::CCDirector::sharedDirector()) - 7.0f,
        cocos2d::CCDirector::getScreenTop(cocos2d::CCDirector::sharedDirector()) - 4.0f
    ));
    m_mainLayer->addChild(m_pageLabel);

    loadPage(0);

    if (isControllerConnected)
    {
        GameToolbox::addRThumbScrollButton(m_mainLayer);
    }
}

void AchievementsLayer::loadPage(int page)
{
    m_currentPage = page;

    m_prevPageButton->setVisible(page > 0);

    cocos2d::CCArray* allAchievements = AchievementManager::sharedState()->m_allAchievementsSorted;

    // Remove steam achievements if not earned
    AchievementManager* am = AchievementManager::sharedState();

    if (!am->isAchievementEarned("geometry.ach.steam01") && allAchievements->count() > 0)
    {
        for (int i = 0; i < allAchievements->count(); i++)
        {
            cocos2d::CCDictionary* dict = (cocos2d::CCDictionary*)allAchievements->objectAtIndex(i);
            cocos2d::CCString* id = (cocos2d::CCString*)dict->valueForKey("identifier");
            std::string idStr = id->getCString();

            if (idStr == "geometry.ach.steam01")
            {
                allAchievements->removeObjectAtIndex(i, true);
                break;
            }
        }
    }

    // Remove steam02 achievement if not earned
    if (!am->isAchievementEarned("geometry.ach.steam02") && allAchievements->count() > 0)
    {
        for (int i = 0; i < allAchievements->count(); i++)
        {
            cocos2d::CCDictionary* dict = (cocos2d::CCDictionary*)allAchievements->objectAtIndex(i);
            cocos2d::CCString* id = (cocos2d::CCString*)dict->valueForKey("identifier");
            std::string idStr = id->getCString();

            if (idStr == "geometry.ach.steam02")
            {
                allAchievements->removeObjectAtIndex(i, true);
                break;
            }
        }
    }

    // Build page array
    cocos2d::CCArray* pageArray = cocos2d::CCArray::create();
    int startIdx = 10 * page;
    int endIdx = startIdx + 10;

    for (int i = startIdx; i < allAchievements->count() && i < endIdx; i++)
    {
        cocos2d::CCObject* obj = allAchievements->objectAtIndex(i);
        pageArray->addObject(obj);
    }

    // Update navigation buttons
    bool hasNext = (endIdx < allAchievements->count());
    m_nextPageButton->setVisible(hasNext);

    // Update page info
    setupPageInfo(allAchievements->count(), startIdx, 10);

    // Setup the list view
    setupLevelBrowser(pageArray);
}

void AchievementsLayer::onNextPage(cocos2d::CCObject* sender)
{
    loadPage(m_currentPage + 1);
}

void AchievementsLayer::onPrevPage(cocos2d::CCObject* sender)
{
    loadPage(m_currentPage - 1);
}

void AchievementsLayer::setupLevelBrowser(cocos2d::CCArray* arr)
{
    if (m_listLayer)
    {
        m_listLayer->removeChildByTag(9, true);
    }

    CustomListView* listView = CustomListView::create(arr, BoomListType::Achievement, 220.0f, 356.0f);
    listView->setTag(9);

    if (m_listLayer)
    {
        m_listLayer->addChild(listView, 6);
    }
}

void AchievementsLayer::setupPageInfo(int itemCount, int pageStartIdx, int pageEndIdx)
{
    m_prevPageButton->setVisible(pageStartIdx != 0);

    int endIdx = pageStartIdx + pageEndIdx;
    if (endIdx > itemCount)
    {
        endIdx = itemCount;
    }

    bool hasNext = (endIdx < itemCount);
    m_nextPageButton->setVisible(hasNext);

    cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
        "%i to %i of %i",
        pageStartIdx + 1,
        endIdx,
        itemCount
    );

    m_pageLabel->setString(str->getCString());
}

AchievementsLayer::~AchievementsLayer()
{
    // CCNode handles cleanup
}