#include "SetupAreaTintTriggerPopup.h"
#include "EnterEffectObject.h"
#include "EffectGameObject.h"
#include "HSVWidgetPopup.h"
#include "GameToolbox.h"
#include "CCMenuItemSpriteExtra.h"

SetupAreaTintTriggerPopup* SetupAreaTintTriggerPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects) {
    SetupAreaTintTriggerPopup* popup = new SetupAreaTintTriggerPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAreaTintTriggerPopup::init(EnterEffectObject* object, cocos2d::CCArray* objects) {
    if (!SetupAreaMoveTriggerPopup::init(object, objects)) {
        return false;
    }

    m_gameObject = object;
    m_gameObjects = objects;

    determineStartValues();

    cocos2d::CCSprite* hsvSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_hsvBtn_001.png");
    m_hsvButton = CCMenuItemSpriteExtra::create(
        hsvSprite, nullptr, menu_selector(SetupAreaTintTriggerPopup::onHSV));
    m_buttonMenu->addChild(m_hsvButton);
    m_hsvButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(m_width * 0.5f - 160.0f, m_height * 0.5f - 30.0f)));

    updateHSVButton();

    return true;
}

void SetupAreaTintTriggerPopup::determineStartValues() {
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject) {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects && m_gameObjects->count() > 0) {
        bool first = true;
        bool mixed = false;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

            if (first) {
                m_hsvValue = obj->m_hsvValue;
                first = false;
            } else {
                if (obj->m_hsvValue.h != m_hsvValue.h ||
                    obj->m_hsvValue.s != m_hsvValue.s ||
                    obj->m_hsvValue.v != m_hsvValue.v ||
                    obj->m_hsvValue.absoluteSaturation != m_hsvValue.absoluteSaturation ||
                    obj->m_hsvValue.absoluteBrightness != m_hsvValue.absoluteBrightness) {
                    m_hsvValue.h = -0.001f;
                    m_hsvValue.s = 0.0f;
                    m_hsvValue.v = 0.0f;
                    m_hsvValue.absoluteSaturation = false;
                    m_hsvValue.absoluteBrightness = false;
                    mixed = true;
                    break;
                }
            }
        }
    }
}

void SetupAreaTintTriggerPopup::onClose(cocos2d::CCObject* sender) {
    if (m_hsvChanged) {
        if (m_gameObject) {
            m_gameObject->m_hsvValue = m_hsvValue;
        } else if (m_gameObjects) {
            for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
                EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
                obj->m_hsvValue = m_hsvValue;
            }
        }
    }
    SetupAreaMoveTriggerPopup::onClose(sender);
}

void SetupAreaTintTriggerPopup::onHSV(cocos2d::CCObject* sender) {
    HSVWidgetPopup* popup = HSVWidgetPopup::create(m_hsvValue, this, "Set HSV");
    popup->show();
}

void SetupAreaTintTriggerPopup::hsvPopupClosed(HSVWidgetPopup* popup, cocos2d::ccHSVValue value) {
    m_hsvValue = value;
    m_hsvChanged = true;
    updateHSVButton();
}

void SetupAreaTintTriggerPopup::updateHSVButton() {
    cocos2d::CCNode* sprite = m_hsvButton->getNormalImage();

    bool isDefault = (m_hsvValue.h == 0.0f && m_hsvValue.s == 0.0f && m_hsvValue.v == 0.0f &&
                      !m_hsvValue.absoluteSaturation && !m_hsvValue.absoluteBrightness);

    const char* frameName = isDefault ? "GJ_hsvBtn_001.png" : "GJ_hsv2Btn_001.png";
    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName);
    ((cocos2d::CCSprite*)sprite)->setDisplayFrame(frame);
}

void SetupAreaTintTriggerPopup::valueDidChange(int tag, float value) {
    SetupAreaMoveTriggerPopup::valueDidChange(tag, value);

    if (tag == 66) {
        if (value != 0.0f) {
            SetupTriggerPopup::updateValue(65, 0.0f);
        }
    } else if (tag == 278) {
        SetupTriggerPopup::toggleGroup(1, value == 0.0f);
        SetupTriggerPopup::toggleGroup(2, value != 0.0f);
    } else if (tag == 65 && value != 0.0f) {
        SetupTriggerPopup::updateValue(66, 0.0f);
    }
}

SetupAreaTintTriggerPopup::~SetupAreaTintTriggerPopup() {
}