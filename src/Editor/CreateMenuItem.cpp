#include "Editor/CreateMenuItem.h"

CreateMenuItem* CreateMenuItem::create(const std::string& spriteFileName,
                                        cocos2d::CCObject* target,
                                        cocos2d::SEL_MenuHandler selector) {
    CreateMenuItem* pRet = new CreateMenuItem();
    if (pRet && pRet->init(spriteFileName, target, selector)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CreateMenuItem::init(const std::string& spriteFileName,
                          cocos2d::CCObject* target,
                          cocos2d::SEL_MenuHandler selector) {
    // Create the sprite from the file name
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(spriteFileName.c_str());
    if (!sprite) {
        return false;
    }
    
    // Initialize base class with the sprite
    if (!CCMenuItemSpriteExtra::initWithNormalSprite(sprite, nullptr, nullptr, target, selector)) {
        return false;
    }
    
    m_spriteFileName = spriteFileName;
    return true;
}

CreateMenuItem::~CreateMenuItem() {
    // m_spriteFileName destructor handles cleanup
}