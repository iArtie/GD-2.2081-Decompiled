#include "GJSpecialColorSelect.h"
#include "ButtonSprite.h"
#include "GJSpecialColorSelectDelegate.h"

ButtonSprite* GJSpecialColorSelect::getButtonByTag(int tag) {
    for (int i = 0; i < m_buttonSprites->count(); ++i) {
        auto sprite = static_cast<ButtonSprite*>(m_buttonSprites->objectAtIndex(i));
        if (sprite->getTag() == tag) {
            return sprite;
        }
    }
    return nullptr;
}

void GJSpecialColorSelect::highlightSelected(ButtonSprite* sprite) {
    for (int i = 0; i < m_buttonSprites->count(); ++i) {
        auto btn = static_cast<ButtonSprite*>(m_buttonSprites->objectAtIndex(i));
        const char* bgImage = (btn->getTag() == 1008) ? "GJ_button_05.png" : "GJ_button_04.png";
        btn->updateBGImage(bgImage);
    }
    
    if (sprite) {
        sprite->updateBGImage("GJ_button_02.png");
    }
}

void GJSpecialColorSelect::keyBackClicked() {
    onClose(nullptr);
}

void GJSpecialColorSelect::onClose(cocos2d::CCObject* sender) {
    if (m_delegate) {
        m_delegate->colorSelectClosed(this, m_colorID);
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJSpecialColorSelect::onSelectColor(cocos2d::CCObject* sender) {
    m_colorID = sender->getTag();
    auto sprite = static_cast<ButtonSprite*>(sender->getNormalImage());
    highlightSelected(sprite);
}

const char* GJSpecialColorSelect::textForColorIdx(int index) {
    switch (index) {
        case 1000: return "BG";
        case 1001: return "G1";
        case 1002: return "L";
        case 1003: return "3DL";
        case 1004: return "Obj";
        case 1005: return "P1";
        case 1006: return "P2";
        case 1007: return "LBG";
        case 1009: return "G2";
        case 1010: return "Black";
        case 1011: return "White";
        case 1012: return "Lighter";
        case 1013: return "MG";
        case 1014: return "MG2";
        default: return "NA";
    }
}

GJSpecialColorSelect::~GJSpecialColorSelect() {
}