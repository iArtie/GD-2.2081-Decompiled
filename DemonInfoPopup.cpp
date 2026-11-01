#include "DemonInfoPopup.h"
#include "GameToolbox.h"
#include "extension/CCScale9Sprite.h"
#include <sstream>

DemonInfoPopup* DemonInfoPopup::createFromString(std::string str)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, ','))
        tokens.push_back(token);
    
    if (tokens.size() < 12)
    {
        return DemonInfoPopup::create(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    
    int easyClassic = atoi(tokens[0].c_str());
    int mediumClassic = atoi(tokens[1].c_str());
    int hardClassic = atoi(tokens[2].c_str());
    int insaneClassic = atoi(tokens[3].c_str());
    int extremeClassic = atoi(tokens[4].c_str());
    int easyPlatformer = atoi(tokens[5].c_str());
    int mediumPlatformer = atoi(tokens[6].c_str());
    int hardPlatformer = atoi(tokens[7].c_str());
    int insanePlatformer = atoi(tokens[8].c_str());
    int extremePlatformer = atoi(tokens[9].c_str());
    int demons = atoi(tokens[10].c_str());
    int weeklyDemons = atoi(tokens[11].c_str());
    
    return DemonInfoPopup::create(
        easyClassic, mediumClassic, hardClassic, insaneClassic, extremeClassic,
        easyPlatformer, mediumPlatformer, hardPlatformer, insanePlatformer, extremePlatformer,
        demons, weeklyDemons
    );
}

DemonInfoPopup* DemonInfoPopup::create(int easyClassic, int mediumClassic, int hardClassic, int insaneClassic, int extremeClassic,
                                        int easyPlatformer, int mediumPlatformer, int hardPlatformer, int insanePlatformer, int extremePlatformer,
                                        int demons, int weeklyDemons)
{
    DemonInfoPopup* pRet = new DemonInfoPopup();
    if (pRet && pRet->init(easyClassic, mediumClassic, hardClassic, insaneClassic, extremeClassic,
                           easyPlatformer, mediumPlatformer, hardPlatformer, insanePlatformer, extremePlatformer,
                           demons, weeklyDemons))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DemonInfoPopup::init(int easyClassic, int mediumClassic, int hardClassic, int insaneClassic, int extremeClassic,
                           int easyPlatformer, int mediumPlatformer, int hardPlatformer, int insanePlatformer, int extremePlatformer,
                           int demons, int weeklyDemons)
{
    if (!FLAlertLayer::init(150))
        return false;
    
    m_easyClassic = easyClassic;
    m_mediumClassic = mediumClassic;
    m_hardClassic = hardClassic;
    m_insaneClassic = insaneClassic;
    m_extremeClassic = extremeClassic;
    m_easyPlatformer = easyPlatformer;
    m_mediumPlatformer = mediumPlatformer;
    m_hardPlatformer = hardPlatformer;
    m_insanePlatformer = insanePlatformer;
    m_extremePlatformer = extremePlatformer;
    m_demons = demons;
    m_weeklyDemons = weeklyDemons;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(360, 220));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Demon Information", "bigFont.fnt");
    title->setScale(0.8f);
    title->setPosition(cocos2d::CCPoint(centerX, centerY + 90.0f));
    m_mainLayer->addChild(title);
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(DemonInfoPopup::onClose));
    closeBtn->setSizeMult(1.6f);
    
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(closeBtn, nullptr);
    menu->setPosition(cocos2d::CCPoint(centerX + 170.0f, centerY + 90.0f));
    m_mainLayer->addChild(menu, 10);
    
    // Display demon counts
    std::string infoText = 
        "Classic:\n"
        "  Easy: " + std::to_string(easyClassic) + "\n"
        "  Medium: " + std::to_string(mediumClassic) + "\n"
        "  Hard: " + std::to_string(hardClassic) + "\n"
        "  Insane: " + std::to_string(insaneClassic) + "\n"
        "  Extreme: " + std::to_string(extremeClassic) + "\n"
        "\nPlatformer:\n"
        "  Easy: " + std::to_string(easyPlatformer) + "\n"
        "  Medium: " + std::to_string(mediumPlatformer) + "\n"
        "  Hard: " + std::to_string(hardPlatformer) + "\n"
        "  Insane: " + std::to_string(insanePlatformer) + "\n"
        "  Extreme: " + std::to_string(extremePlatformer) + "\n"
        "\nTotal Demons: " + std::to_string(demons) + "\n"
        "Weekly Demons: " + std::to_string(weeklyDemons);
    
    TextArea* textArea = TextArea::create(infoText, "chatFont.fnt", 1.0f, 9999.0f, cocos2d::CCPoint(0, 0), 0.0f, false);
    textArea->setScale(0.5f);
    m_mainLayer->addChild(textArea);
    textArea->setPosition(cocos2d::CCPoint(centerX, centerY + 25.0f));
    
    return true;
}

void DemonInfoPopup::keyBackClicked()
{
    onClose(nullptr);
}

void DemonInfoPopup::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

DemonInfoPopup::~DemonInfoPopup()
{
    // Base class handles cleanup
}