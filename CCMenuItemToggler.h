#pragma once

#include "cocos2d.h"
#include "CCMenuItemSpriteExtra.h"

class CCMenuItemToggler : public cocos2d::CCMenuItem {
public:
    static CCMenuItemToggler* create(cocos2d::CCNode* offButton, cocos2d::CCNode* onButton, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);

    virtual void activate() override;
    virtual void selected() override;
    virtual void unselected() override;
    virtual void setEnabled(bool enabled) override;

    CCMenuItemSpriteExtra* activeItem();
    void normalTouch(cocos2d::CCObject* sender);
    void selectedTouch(cocos2d::CCObject* sender);
    void setSizeMult(float mult);
    void toggle(bool on);

    bool isToggled() const { return m_toggled; }

    ~CCMenuItemToggler();

private:
    CCMenuItemSpriteExtra* m_offButton;
    CCMenuItemSpriteExtra* m_onButton;
    bool m_toggled;
    bool m_notClickable;
};