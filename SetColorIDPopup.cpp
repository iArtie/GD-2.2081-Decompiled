#include "SetColorIDPopup.h"
#include "GJSpecialColorSelect.h"
#include "CCTextInputNode.h"
#include "CCMenuItemSpriteExtra.h"

SetColorIDPopup* SetColorIDPopup::create(int id) {
    SetColorIDPopup* popup = new SetColorIDPopup();
    if (popup && popup->init(id)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetColorIDPopup::init(int id) {
    if (!SetIDPopup::init(id, 0, 1101, "Color Filter", "OK", true, false, 0.0f, true, false)) {
        return false;
    }

    cocos2d::CCSprite* plusSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    CCMenuItemSpriteExtra* plusButton = CCMenuItemSpriteExtra::create(
        plusSprite, nullptr, menu_selector(SetColorIDPopup::onSelectSpecialColor));
    m_buttonMenu->addChild(plusButton);
    plusButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        m_inputNode->getPosition() + cocos2d::CCPoint(75.0f, 0.0f)));

    if (id > 999) {
        std::string colorText = GJSpecialColorSelect::textForColorIdx(id);
        m_inputNode->updateLabel(colorText);
    }

    return true;
}

void SetColorIDPopup::colorSelectClosed(GJSpecialColorSelect* select, int id) {
    if (id <= 999) {
        return;
    }

    m_disableDelegate = true;
    m_value = id;

    m_inputNode->setString("0");
    m_inputNode->updateLabel(GJSpecialColorSelect::textForColorIdx(id));

    valueChanged(this);

    m_disableDelegate = false;
}

void SetColorIDPopup::onSelectSpecialColor(cocos2d::CCObject* sender) {
    GJSpecialColorSelect* select = GJSpecialColorSelect::create(
        m_value, this, GJSpecialColorSelectType::Filter);
    select->setTag(sender->getTag());
    select->show();
}

SetColorIDPopup::~SetColorIDPopup() {
}