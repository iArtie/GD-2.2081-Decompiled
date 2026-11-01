#include "EditGameObjectPopup.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "extension/CCScale9Sprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"
#include "Slider.h"
#include "SliderThumb.h"
#include "CCTextInputNode.h"
#include "ColorSelectPopup.h"
#include "HSVWidgetPopup.h"
#include "ConfigureValuePopup.h"
#include "InfoAlertButton.h"

EditGameObjectPopup* EditGameObjectPopup::create(EffectGameObject* object, cocos2d::CCArray* objects, bool platformer)
{
    EditGameObjectPopup* pRet = new EditGameObjectPopup();
    if (pRet && pRet->init(object, objects, platformer))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool EditGameObjectPopup::init(EffectGameObject* object, cocos2d::CCArray* objects, bool platformer)
{
    if (!FLAlertLayer::init(150))
        return false;
    
    m_targetObject = object;
    m_targetObjects = objects;
    if (objects) objects->retain();
    m_platformerMode = platformer;
    
    m_unknownBool1 = false;
    m_unknownBool2 = false;
    m_unknownBool3 = false;
    m_unknownBool4 = false;
    m_unknownInt1 = 0;
    m_unknownInt2 = 0;
    m_unknownInt3 = 0;
    m_unknownInt4 = 0;
    m_unknownInt5 = 0;
    m_unknownFloat1 = 0.0f;
    m_unknownFloat2 = 1.0f;
    m_delegate = nullptr;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(360, 250));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    // Title
    m_titleLabel = cocos2d::CCLabelBMFont::create("Edit Object", "bigFont.fnt");
    m_titleLabel->setScale(0.8f);
    m_titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 100.0f));
    m_mainLayer->addChild(m_titleLabel);
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(EditGameObjectPopup::onClose));
    closeBtn->setSizeMult(1.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(centerX + 170.0f, centerY + 100.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Add object-specific controls based on object type
    // This would include sliders, text inputs, color pickers, etc.
    
    // OK button
    ButtonSprite* okSprite = ButtonSprite::create("OK");
    CCMenuItemSpriteExtra* okBtn = CCMenuItemSpriteExtra::create(
        okSprite, nullptr, this, menu_selector(EditGameObjectPopup::onClose));
    m_buttonMenu->addChild(okBtn);
    okBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(centerX, centerY - 120.0f + 25.0f)));
    
    return true;
}

EditGameObjectPopup::~EditGameObjectPopup()
{
    CC_SAFE_RELEASE(m_targetObjects);
}