#include "Cells/GameCell.h"
#include "Platform/cocos2d.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GameCell::GameCell(const char* charPtr, float value, float value2) 
    : TableViewCell(charPtr, value, value2) {
    m_gameLink = "";
    init();
}

void GameCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), 
                        cocos2d::CCPoint(size.width - 1.0f, size.height));
}

bool GameCell::init() {
    m_unk230 = 0;
    return true;
}

void GameCell::loadFromString(std::string str) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    
    // Determine the game link based on the string prefix
    std::string link;
    
    if (str.find("gj") == 0) {
        link = "https://www.geometrydash.com/game/";
    } else if (str.find("gjl") == 0) {
        link = "https://www.geometrydash.com/game/";
    } else if (str.find("gjm") == 0) {
        link = "https://www.geometrydash.com/game/";
    } else if (str.find("gjw") == 0) {
        link = "https://www.geometrydash.com/game/";
    } else if (str.find("gjz") == 0) {
        link = "https://www.geometrydash.com/game/";
    } else {
        link = "https://www.geometrydash.com/game/";
    }
    
    m_gameLink = link;
    
    // Create sprite with promo image
    cocos2d::CCString* spriteName = cocos2d::CCString::createWithFormat("promo_%s.png", str.c_str());
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(spriteName->getCString());
    
    // Create menu item
    CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(sprite, nullptr, this, 
        menu_selector(GameCell::onTouch));
    menuItem->m_colorEnabled = true;
    menuItem->m_animationEnabled = false;
    
    // Create menu
    cocos2d::CCNode* menu = cocos2d::CCMenu::create(menuItem, nullptr);
    m_mainLayer->addChild(menu);
    menu->setPosition(cocos2d::CCPoint(m_width * 0.5f, m_height * 0.5f));
}

void GameCell::onTouch(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL(m_gameLink.c_str());
}

void GameCell::updateBGColor(int index) {
    if (index & 1) {
        m_backgroundLayer->setColor(cocos2d::ccColor3B{255, 255, 255});
    } else {
        m_backgroundLayer->setColor(cocos2d::ccColor3B{0, 0, 255});
    }
    m_backgroundLayer->setOpacity(255);
}

GameCell::~GameCell() {
    // Base class destructor handles cleanup
}