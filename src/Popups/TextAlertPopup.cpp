#include "Popups/TextAlertPopup.h"

TextAlertPopup* TextAlertPopup::create(std::string text, float delay, float scale, int opacity, std::string font) {
    TextAlertPopup* returnVal = new TextAlertPopup();
    if (returnVal->init(text, delay, scale, opacity, font)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool TextAlertPopup::init(std::string text, float delay, float scale, int opacity, std::string font) {
    m_label = cocos2d::CCLabelBMFont::create(text.c_str(), font.c_str());
    if (!m_label)
        return false;

    m_label->setScale(scale);
    m_label->setOpacity(opacity);
    addChild(m_label);

    cocos2d::CCAction* fadeIn = cocos2d::CCFadeIn::create(0.3f);
    cocos2d::CCAction* wait = cocos2d::CCDelayTime::create(delay);
    cocos2d::CCAction* fadeOut = cocos2d::CCFadeOut::create(0.5f);
    cocos2d::CCAction* remove = cocos2d::CCRemoveSelf::create();
    m_label->runAction(cocos2d::CCSequence::create(fadeIn, wait, fadeOut, remove, nullptr));

    return true;
}

TextAlertPopup::~TextAlertPopup() {
}