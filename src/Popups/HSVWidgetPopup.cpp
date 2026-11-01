#include "Popups/HSVWidgetPopup.h"
#include "Editor/ConfigureHSVWidget.h"
#include "Editor/HSVWidgetDelegate.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

HSVWidgetPopup* HSVWidgetPopup::create(cocos2d::ccHSVValue hsv, HSVWidgetDelegate* delegate, const std::string& title) {
    auto popup = new HSVWidgetPopup();
    if (popup && popup->init(hsv, delegate, title)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool HSVWidgetPopup::init(cocos2d::ccHSVValue hsv, HSVWidgetDelegate* delegate, const std::string& title) {
    if (!FLAlertLayer::init(200)) {
        return false;
    }
    
    m_delegate = delegate;
    m_noElasticity = true;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(360.0f, 250.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    std::string titleText = title.empty() ? "Setup HSV" : title;
    auto titleLabel = cocos2d::CCLabelBMFont::create(titleText.c_str(), "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 102.0f));
    m_mainLayer->addChild(titleLabel);
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    auto okBtn = ButtonSprite::create("OK", 0, 0, 1.0f);
    auto okItem = CCMenuItemSpriteExtra::create(okBtn, this, menu_selector(HSVWidgetPopup::onClose));
    m_buttonMenu->addChild(okItem);
    okItem->setPosition(cocos2d::CCPoint(centerX, centerY - 100.0f));
    
    m_widget = ConfigureHSVWidget::create(hsv, 0, true);
    m_mainLayer->addChild(m_widget, 1);
    m_widget->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    return true;
}

void HSVWidgetPopup::keyBackClicked() {
    onClose(nullptr);
}

void HSVWidgetPopup::onClose(cocos2d::CCObject* sender) {
    m_widget->onClose();
    
    if (!m_widget->m_mixed && m_delegate) {
        m_delegate->hsvPopupClosed(this, m_widget->m_hsv);
    }
    
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

HSVWidgetPopup::~HSVWidgetPopup() {
}