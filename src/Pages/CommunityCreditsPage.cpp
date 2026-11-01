#include "Pages/CommunityCreditsPage.h"
#include "UI/GJItemIcon.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

void CommunityCreditsPage::FLAlert_Clicked(FLAlertLayer* alert, bool btn2) {
    // Empty implementation
}

CommunityCreditsPage* CommunityCreditsPage::create() {
    CommunityCreditsPage* pRet = new CommunityCreditsPage();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void CommunityCreditsPage::goToPage(int page) {
    int count = m_pageObjects->count();
    if (page >= 0 && page < count && m_page != page) {
        m_page = page;
        
        for (int i = 0; i < count; i++) {
            cocos2d::CCObject* obj = m_pageObjects->objectForKey(
                cocos2d::CCString::createWithFormat("%d", i));
            if (obj) {
                // Iterate through children and set visibility based on page
                cocos2d::CCArray* children = obj->getChildren();
                if (children) {
                    for (unsigned int j = 0; j < children->count(); j++) {
                        cocos2d::CCNode* child = (cocos2d::CCNode*)children->objectAtIndex(j);
                        child->setVisible(page == i);
                    }
                }
            }
        }
        
        m_prevButton->setVisible(page > 0);
        m_nextButton->setVisible(page < count - 1);
    }
}

bool CommunityCreditsPage::init() {
    if (!FLAlertLayer::init(150)) {
        return false;
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(420, 280));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    // Corner decorations
    for (int i = 1; i <= 4; i++) {
        cocos2d::CCSprite* corner = cocos2d::CCSprite::createWithSpriteFrameName(
            "dailyLevelCorner_001.png");
        m_mainLayer->addChild(corner, 8);
        
        float xOffset = 0.0f;
        float yOffset = 0.0f;
        
        switch (i) {
            case 1:
                xOffset = -185.0f;
                yOffset = -115.0f;
                break;
            case 2:
                corner->setFlipY(true);
                xOffset = -185.0f;
                yOffset = 115.0f;
                break;
            case 3:
                corner->setFlipX(true);
                corner->setFlipY(true);
                xOffset = 185.0f;
                yOffset = 115.0f;
                break;
            case 4:
                corner->setFlipX(true);
                xOffset = 185.0f;
                yOffset = -115.0f;
                break;
        }
        corner->setPosition(cocos2d::CCPoint(centerX + xOffset, centerY + yOffset));
    }
    
    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Community Credits", "bigFont.fnt");
    title->setScale(0.7f);
    m_mainLayer->addChild(title);
    title->setPosition(cocos2d::CCPoint(centerX, centerY + 140.0f - 20.0f));
    
    // Create page dictionary
    m_pageObjects = cocos2d::CCDictionary::create();
    m_pageObjects->retain();
    
    // Create community credit nodes (hardcoded list from decompiled code)
    cocos2d::CCArray* page0 = cocos2d::CCArray::create();
    // Add all the hardcoded CommunityCreditNode entries
    // This is a massive list - I'll summarize the pattern
    
    // Pattern for creating credit nodes:
    // CommunityCreditNode::create(iconType, iconID, 0, username)
    // Icon types: 1=Cube, 4=Ship, 5=Ball, 6=Ufo, 7=Wave, 8=Robot, 9=Spider
    
    std::vector<std::pair<int, std::pair<int, std::string>>> credits = {
        {1, {117, "Aleck08"}},
        {1, {118, "Aruxd"}},
        {1, {119, "RadleyMCT"}},
        {1, {120, "Djoxy"}},
        {1, {121, "ghostpower"}},
        {1, {122, "hallymallybubu"}},
        {1, {123, "Ic3Fir3"}},
        {1, {124, "ItsJavi2504x"}},
        {1, {125, "LaZye"}},
        {1, {126, "Last67706"}},
        {1, {127, "luisilloRS"}},
        {1, {128, "luisilloRS"}},
        {1, {129, "NeXuSGamer"}},
        {1, {130, "RobBuck"}},
        {1, {131, "RobBuck"}},
        {1, {132, "RobBuck"}},
        {1, {133, "SenpaiShika"}},
        {1, {134, "SenpaiShika"}},
        {1, {135, "Terron"}},
        {5, {30, "Cygnus"}},
        {5, {31, "slushys"}},
        {5, {32, "Deraklich"}},
        {5, {33, "DracTheDragon"}},
        {5, {34, "DaRealDazzer"}},
        {5, {35, "Mariolego320"}},
        {5, {36, "Mariolego320"}},
        {5, {37, "Revanty"}},
        {5, {38, "checho623"}},
        {4, {36, "Aleck08"}},
        {4, {37, "Alex PG"}},
        {4, {38, "Edialex"}},
        {4, {39, "ItsSunset"}},
        {4, {40, "luisilloRS"}},
        {4, {41, "luisilloRS"}},
        {4, {42, "NeXuSGamer"}},
        {4, {43, "Plazda"}},
        {4, {44, "RagnokGD"}},
        {4, {45, "TheRailgunner"}},
        {4, {46, "RobBuck"}},
        {4, {47, "ScorchVx"}},
        {4, {48, "Virusmetal"}},
        {6, {29, "DaferSilver"}},
        {6, {30, "Samuraivi13"}},
        {6, {31, "haoN"}},
        {6, {32, "luisilloRS"}},
        {6, {33, "ScorchVx"}},
        {6, {34, "ScorchVx"}},
        {6, {35, "ScorchVx"}},
        {7, {24, "SpaceSyrup"}},
        {7, {25, "Arachnus"}},
        {7, {26, "CyverMast"}},
        {7, {27, "lordwerthers"}},
        {7, {28, "Jntx231"}},
        {7, {29, "RobBuck"}},
        {7, {30, "DarkFireDrago"}},
        {7, {31, "ScorchVx"}},
        {7, {32, "SNSGaming"}},
        {7, {33, "xVexis"}},
        {8, {18, "Splons"}},
        {8, {19, "k3vo5ky"}},
        {8, {20, "luisilloRS"}},
        {8, {21, "Mariolego320"}},
        {8, {22, "ScorchVx"}},
        {8, {23, "ScorchVx"}},
        {8, {24, "Shacal"}},
        {8, {25, "Tr1a"}},
        {9, {11, "DaferSilver"}},
        {9, {12, "Edialex"}},
        {9, {13, "Edialex"}},
        {9, {14, "Litexotic"}},
        {9, {15, "ScorchVx"}},
        {9, {16, "checho623"}}
    };
    
    int idx = 0;
    int maxPage = 0;
    
    for (const auto& credit : credits) {
        int iconType = credit.first;
        int iconID = credit.second.first;
        const std::string& name = credit.second.second;
        
        int page = idx / 12;
        
        cocos2d::CCArray* pageArray = (cocos2d::CCArray*)m_pageObjects->objectForKey(
            cocos2d::CCString::createWithFormat("%d", page));
        if (!pageArray) {
            pageArray = cocos2d::CCArray::create();
            m_pageObjects->setObject(pageArray, cocos2d::CCString::createWithFormat("%d", page));
        }
        if (page > maxPage) maxPage = page;
        
        CommunityCreditNode* node = CommunityCreditNode::create(iconType, iconID, 0, name);
        
        // Position in grid (4 columns, rows of 3)
        int row = (idx % 12) / 4;
        int col = idx % 4;
        float x = centerX + (col - 1.5f) * 85.0f;
        float y = centerY + 85.0f - row * 80.0f;
        
        m_mainLayer->addChild(node, 3);
        node->setPosition(cocos2d::CCPoint(x, y));
        pageArray->addObject(node);
        
        // Name label
        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
        label->limitLabelWidth(60.0f, 0.6f, 0.0f);
        m_mainLayer->addChild(label, 3);
        label->setPosition(cocos2d::CCPoint(x, y - 34.0f));
        pageArray->addObject(label);
        
        idx++;
    }
    
    // Add special community icons page
    cocos2d::CCArray* specialPage = cocos2d::CCArray::create();
    m_pageObjects->setObject(specialPage, cocos2d::CCString::createWithFormat("%d", maxPage + 1));
    
    cocos2d::CCLabelBMFont* specialLabel = cocos2d::CCLabelBMFont::create("Special Thanks", "bigFont.fnt");
    specialLabel->setColor(cocos2d::ccColor3B(50, 200, 255));
    specialLabel->setScale(0.6f);
    specialLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 80.0f));
    m_mainLayer->addChild(specialLabel);
    specialPage->addObject(specialLabel);
    
    // Special contributors
    const char* specialContributors[] = {"Etzer", "Michigun", "ViPriN"};
    cocos2d::CCPoint basePos(centerX, centerY);
    
    for (int i = 0; i < 3; i++) {
        cocos2d::CCSprite* icon = cocos2d::CCSprite::createWithSpriteFrameName(
            cocos2d::CCString::createWithFormat("communityIcon_%02d_001.png", i + 1)->getCString());
        m_mainLayer->addChild(icon, 3);
        
        cocos2d::CCPoint pos = basePos;
        pos.x += (i - 1) * 90.0f;
        pos.y += (i == 0) ? 5.0f : 0.0f;
        icon->setPosition(pos);
        specialPage->addObject(icon);
        
        cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create(specialContributors[i], "bigFont.fnt");
        nameLabel->limitLabelWidth(60.0f, 0.6f, 0.0f);
        m_mainLayer->addChild(nameLabel, 3);
        nameLabel->setPosition(cocos2d::CCPoint(pos.x, pos.y + 40.0f));
        specialPage->addObject(nameLabel);
        
        // Heart icon for Michigun
        if (i == 1) {
            cocos2d::CCSprite* heart = cocos2d::CCSprite::createWithSpriteFrameName("d_heart01_001.png");
            heart->setColor(cocos2d::ccColor3B(0, 255, 0));
            heart->setScale(0.4f);
            m_mainLayer->addChild(heart, 3);
            heart->setPosition(cocos2d::CCPoint(pos.x + 40.0f, pos.y + 5.0f));
            specialPage->addObject(heart);
        }
    }
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(CommunityCreditsPage::onClose));
    closeBtn->setSizeMult(1.6f);
    
    cocos2d::CCMenu* closeMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu = closeMenu;
    closeMenu->setPosition(cocos2d::CCPoint(centerX - 210.0f + 10.0f, centerY + 140.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Navigation buttons
    cocos2d::CCMenu* navMenu = cocos2d::CCMenu::create();
    
    // Previous button
    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevButton = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, menu_selector(CommunityCreditsPage::onSwitchPage));
    m_prevButton->setTag(0);
    m_prevButton->setSizeMult(1.4f);
    navMenu->addChild(m_prevButton);
    
    cocos2d::CCPoint prevPos = cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f;
    prevPos.y = centerY;
    m_prevButton->setPosition(navMenu->convertToNodeSpace(prevPos));
    
    // Next button
    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(CommunityCreditsPage::onSwitchPage));
    m_nextButton->setTag(1);
    m_nextButton->setSizeMult(1.4f);
    navMenu->addChild(m_nextButton);
    
    cocos2d::CCPoint nextPos = cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f;
    nextPos.y = centerY;
    m_nextButton->setPosition(navMenu->convertToNodeSpace(nextPos));
    
    m_mainLayer->addChild(navMenu, 10);
    
    goToPage(0);
    
    return true;
}

void CommunityCreditsPage::keyBackClicked() {
    onClose(nullptr);
}

void CommunityCreditsPage::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void CommunityCreditsPage::onSwitchPage(cocos2d::CCObject* sender) {
    int newPage = m_page;
    if (sender->getTag() == 1) {
        newPage++;
    } else {
        newPage--;
    }
    goToPage(newPage);
}

void CommunityCreditsPage::registerWithTouchDispatcher() {
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void CommunityCreditsPage::show() {
    if (!m_noElasticity) {
        cocos2d::CCPoint originalPos = m_mainLayer->getPosition();
        unsigned char opacity = getOpacity();
        
        m_mainLayer->setPosition(cocos2d::CCPoint(originalPos.x, 160.0f));
        
        cocos2d::CCAction* moveAction = cocos2d::CCEaseElasticOut::create(
            cocos2d::CCMoveTo::create(0.5f, originalPos), 0.6f);
        m_mainLayer->runAction(moveAction);
        
        cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
        if (m_scene && m_ZOrder) {
            m_scene->addChild(this, m_ZOrder);
        } else {
            m_scene = scene;
            m_ZOrder = scene->getHighestChildZ() + 1;
            if (m_ZOrder < 105) m_ZOrder = 105;
            m_scene->addChild(this, m_ZOrder);
        }
        
        setOpacity(0);
        cocos2d::CCAction* fadeAction = cocos2d::CCFadeTo::create(0.14f, opacity);
        runAction(fadeAction);
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

CommunityCreditsPage::~CommunityCreditsPage() {
    CC_SAFE_RELEASE(m_pageObjects);
}