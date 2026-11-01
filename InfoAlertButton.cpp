#include "InfoAlertButton.h"
#include "FLAlertLayer.h"

InfoAlertButton* InfoAlertButton::create(const std::string& title, const std::string& desc, float spriteScale) {
    auto button = new InfoAlertButton();
    if (button && button->init(title, desc, spriteScale)) {
        button->autorelease();
        return button;
    }
    delete button;
    return nullptr;
}

bool InfoAlertButton::init(const std::string& title, const std::string& desc, float spriteScale) {
    auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    sprite->setScale(spriteScale);
    
    if (!CCMenuItemSpriteExtra::init(sprite, nullptr, nullptr, nullptr)) {
        return false;
    }
    
    setSizeMult(1.5f);
    m_title = title;
    m_description = desc;
    m_bEnabled = true;
    m_scroll = false;
    m_textScale = 1.0f;
    
    return true;
}

void InfoAlertButton::activate() {
    if (!m_bEnabled) {
        return;
    }
    
    auto alert = FLAlertLayer::create(
        nullptr,
        m_title.c_str(),
        m_description.c_str(),
        "OK",
        nullptr,
        430.0f,
        m_scroll,
        m_textScale
    );
    alert->show();
}

InfoAlertButton::~InfoAlertButton() {
}