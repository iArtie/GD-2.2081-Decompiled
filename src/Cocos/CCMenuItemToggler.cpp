#include "Cocos/CCMenuItemToggler.h"
#include "Platform/cocos2d.h"

CCMenuItemToggler* CCMenuItemToggler::create(cocos2d::CCNode* offButton, cocos2d::CCNode* onButton, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    CCMenuItemToggler* item = new CCMenuItemToggler();
    if (item && item->initWithTarget(target, selector)) {
        item->m_offButton = static_cast<CCMenuItemSpriteExtra*>(offButton);
        item->m_onButton = static_cast<CCMenuItemSpriteExtra*>(onButton);
        item->m_offButton->retain();
        item->m_onButton->retain();
        item->addChild(offButton);
        item->addChild(onButton);
        item->toggle(false);
        item->autorelease();
        return item;
    }
    delete item;
    return nullptr;
}

void CCMenuItemToggler::activate() {
    cocos2d::CCMenuItem::activate();
    CCMenuItemSpriteExtra* item = activeItem();
    if (item) {
        item->activate();
    }
}

CCMenuItemSpriteExtra* CCMenuItemToggler::activeItem() {
    return m_toggled ? m_onButton : m_offButton;
}

void CCMenuItemToggler::normalTouch(cocos2d::CCObject* sender) {
    if (!m_notClickable) {
        toggle(true);
    }
}

void CCMenuItemToggler::selected() {
    cocos2d::CCMenuItem::selected();
    CCMenuItemSpriteExtra* item = activeItem();
    if (item) {
        item->selected();
    }
}

void CCMenuItemToggler::selectedTouch(cocos2d::CCObject* sender) {
    if (!m_notClickable) {
        toggle(false);
    }
}

void CCMenuItemToggler::setEnabled(bool enabled) {
    cocos2d::CCMenuItem::setEnabled(enabled);
    m_offButton->setEnabled(enabled);
    m_onButton->setEnabled(enabled);
}

void CCMenuItemToggler::setSizeMult(float mult) {
    m_offButton->setSizeMult(mult);
    m_onButton->setSizeMult(mult);
    toggle(m_toggled);
}

void CCMenuItemToggler::toggle(bool on) {
    m_toggled = on;

    m_offButton->setVisible(!on);
    m_onButton->setVisible(on);

    CCMenuItemSpriteExtra* item = activeItem();
    if (item) {
        cocos2d::CCSize size = item->getContentSize();
        this->setContentSize(size);

        cocos2d::CCPoint center(size.width * 0.5f, size.height * 0.5f);
        m_offButton->setPosition(center);
        m_onButton->setPosition(center);
    }
}

void CCMenuItemToggler::unselected() {
    cocos2d::CCMenuItem::unselected();
    CCMenuItemSpriteExtra* item = activeItem();
    if (item) {
        item->unselected();
    }
}

CCMenuItemToggler::~CCMenuItemToggler() {
    CC_SAFE_RELEASE_NULL(m_offButton);
    CC_SAFE_RELEASE_NULL(m_onButton);
}