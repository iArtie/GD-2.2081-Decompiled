#include "DemonFilterSelectLayer.h"
#include "GameLevelManager.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "extension/CCScale9Sprite.h"
#include "CCMenuItemSpriteExtra.h"

DemonFilterSelectLayer* DemonFilterSelectLayer::create()
{
    DemonFilterSelectLayer* pRet = new DemonFilterSelectLayer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DemonFilterSelectLayer::init()
{
    if (!FLAlertLayer::init(75))
        return false;
    
    m_delegate = nullptr;
    m_currentDemon = 0;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(380, 180));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Demon Filter", "bigFont.fnt");
    title->setPosition(cocos2d::CCPoint(centerX, centerY + 90.0f - 20.0f));
    m_mainLayer->addChild(title);
    
    m_demons = cocos2d::CCArray::create();
    m_demons->retain();
    
    cocos2d::CCArray* tempArray = cocos2d::CCArray::create();
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Get current demon filter
    int currentFilter = GameLevelManager::sharedState()->getIntForKey("demon_filter");
    cocos2d::CCObject* selectedObj = nullptr;
    
    // Demon difficulty values: 0 = Easy, 1 = Medium, 2 = Hard, 3 = Insane, 4 = Extreme, 5 = Auto
    int demonValues[] = {0, 1, 2, 3, 4, 5};
    const char* buttonTexture = "difficulty_06_btn_001.png";
    const char* formatString = "difficulty_%02d_btn2_001.png";
    
    for (int i = 0; i < 6; i++)
    {
        std::string spriteName;
        int demonValue = demonValues[i];
        
        if (i == 0)
        {
            spriteName = buttonTexture;
        }
        else
        {
            spriteName = cocos2d::CCString::createWithFormat(formatString, demonValue)->getCString();
        }
        
        cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(spriteName);
        sprite->setScale(1.1f);
        
        CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(
            sprite, nullptr, this, menu_selector(DemonFilterSelectLayer::selectRating));
        m_buttonMenu->addChild(menuItem);
        m_demons->addObject(menuItem);
        tempArray->addObject(menuItem);
        menuItem->setTag(demonValue);
        
        if (demonValue == currentFilter)
            selectedObj = menuItem;
    }
    
    // Align buttons horizontally
    cocos2d::CCPoint alignPos = cocos2d::CCPoint(centerX, centerY + 3.0f);
    alignPos = m_buttonMenu->convertToNodeSpace(alignPos);
    GameToolbox::alignItemsHorisontally(tempArray, 18.0f, alignPos, false);
    tempArray->removeAllObjects();
    
    // OK button
    ButtonSprite* okSprite = ButtonSprite::create("OK");
    CCMenuItemSpriteExtra* okBtn = CCMenuItemSpriteExtra::create(
        okSprite, nullptr, this, menu_selector(DemonFilterSelectLayer::onClose));
    m_buttonMenu->addChild(okBtn);
    tempArray->addObject(okBtn);
    okBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(centerX, centerY - 90.0f + 25.0f)));
    
    if (selectedObj)
        selectRating(selectedObj);
    
    return true;
}

void DemonFilterSelectLayer::keyBackClicked()
{
    onClose(nullptr);
}

void DemonFilterSelectLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
    
    if (m_delegate)
        m_delegate->demonFilterSelectClosed(m_currentDemon);
}

void DemonFilterSelectLayer::selectRating(cocos2d::CCObject* sender)
{
    m_currentDemon = sender->getTag(sender);
    
    // Update all buttons to unselected state
    for (unsigned int i = 0; i < m_demons->count(); i++)
    {
        cocos2d::CCObject* obj = m_demons->objectAtIndex(i);
        ((CCMenuItemSpriteExtra*)obj)->setEnabled(true);
    }
    
    // Disable the selected button
    ((CCMenuItemSpriteExtra*)sender)->setEnabled(false);
}

DemonFilterSelectLayer::~DemonFilterSelectLayer()
{
    CC_SAFE_RELEASE(m_demons);
}