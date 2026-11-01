#include "HSVLiveOverlay.h"
#include "GameObject.h"
#include "GJSpriteColor.h"
#include "ConfigureHSVWidget.h"
#include "ColorSelectDelegate.h"
#include "HSVWidgetDelegate.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

HSVLiveOverlay* HSVLiveOverlay::create(GameObject* object, cocos2d::CCArray* objects) {
    auto overlay = new HSVLiveOverlay();
    if (overlay && overlay->init(object, objects)) {
        overlay->autorelease();
        return overlay;
    }
    delete overlay;
    return nullptr;
}

bool HSVLiveOverlay::init(GameObject* object, cocos2d::CCArray* objects) {
    if (!FLAlertLayer::init(nullptr)) {
        return false;
    }
    
    if (object) {
        m_object = object;
        m_object->retain();
    } else if (objects) {
        m_objects = objects;
        m_objects->retain();
    }
    
    m_noElasticity = true;
    m_activeTab = -1;
    m_unkBool1 = false;
    m_unkBool2 = false;
    m_unkBool3 = false;
    m_delegate = nullptr;
    m_widget = nullptr;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    director->getWinSize();
    
    m_controls = cocos2d::CCArray::create();
    m_controls->retain();
    
    setTouchEnabled(false);
    setKeypadEnabled(false);
    
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.5f);
    auto closeItem = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(HSVLiveOverlay::closeColorSelect));
    
    m_buttonMenu = cocos2d::CCMenu::create(closeItem, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->setPosition(cocos2d::CCPoint(157.0f - 81.0f, 163.0f + 48.0f));
    
    determineStartValues();
    
    if (m_unkBool1 && m_unkBool2) {
        m_unkArray = cocos2d::CCArray::create();
        m_unkArray->retain();
        
        auto btn1 = ButtonSprite::create("1", 0, 1, 0.5f, true, "bigFont.fnt", "GJ_button_01.png", nullptr, 30.0f);
        btn1->setScale(0.6f);
        auto item1 = CCMenuItemSpriteExtra::create(btn1, this, menu_selector(HSVLiveOverlay::onSelectTab));
        m_buttonMenu->addChild(item1);
        item1->setTag(1);
        btn1->setTag(1);
        m_unkArray->addObject(btn1);
        
        auto btn2 = ButtonSprite::create("2", 0, 1, 0.5f, true, "bigFont.fnt", "GJ_button_04.png", nullptr, 30.0f);
        btn2->setScale(0.6f);
        auto item2 = CCMenuItemSpriteExtra::create(btn2, this, menu_selector(HSVLiveOverlay::onSelectTab));
        m_buttonMenu->addChild(item2);
        item2->setTag(2);
        btn2->setTag(2);
        m_unkArray->addObject(btn2);
        
        cocos2d::CCPoint pos1 = m_mainLayer->convertToWorldSpace(cocos2d::CCPoint(157.0f + 97.0f, 163.0f + 36.0f));
        item1->setPosition(m_buttonMenu->convertToNodeSpace(pos1));
        
        cocos2d::CCPoint pos = item1->getPosition();
        item2->setPosition(pos + cocos2d::CCPoint(0.0f, -24.0f));
    }
    
    int initialTab = m_unkBool1 ? 1 : 2;
    createHSVWidget(initialTab);
    
    return true;
}

void HSVLiveOverlay::determineStartValues() {
    if (!m_object) {
        for (int i = 0; i < m_objects->count(); ++i) {
            auto obj = m_objects->objectAtIndex(i);
            auto gameObj = static_cast<GameObject*>(obj);
            
            if (!m_unkBool1) {
                m_unkBool1 = GameObject::getRelativeSpriteColor(gameObj, 1) != nullptr;
            }
            if (!m_unkBool2) {
                m_unkBool2 = GameObject::getRelativeSpriteColor(gameObj, 2) != nullptr;
            }
            
            if (m_unkBool1 && m_unkBool2) {
                return;
            }
        }
        return;
    }
    
    m_unkBool1 = GameObject::getRelativeSpriteColor(m_object, 1) != nullptr;
    m_unkBool2 = GameObject::getRelativeSpriteColor(m_object, 2) != nullptr;
}

void HSVLiveOverlay::createHSVWidget(int tab) {
    m_activeTab = tab;
    
    if (m_unkArray) {
        for (int i = 0; i < m_unkArray->count(); ++i) {
            auto sprite = m_unkArray->objectAtIndex(i);
            auto btn = static_cast<ButtonSprite*>(sprite);
            const char* bg = (tab == btn->getTag()) ? "GJ_button_01.png" : "GJ_button_04.png";
            btn->updateBGImage(bg);
        }
    }
    
    if (m_widget) {
        m_widget->removeMeAndCleanup();
        m_widget = nullptr;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    director->getWinSize();
    
    cocos2d::ccHSVValue hsv = ConfigureHSVWidget::getHSV(m_object, m_objects, tab);
    m_widget = ConfigureHSVWidget::create(hsv, 0, 0);
    m_widget->setTag(tab);
    m_mainLayer->addChild(m_widget);
    m_widget->setPosition(cocos2d::CCPoint(157.0f, 163.0f));
    m_widget->setScale(0.6f);
    m_widget->m_delegate = this;
}

void HSVLiveOverlay::hsvChanged(ConfigureHSVWidget* widget) {
    int colorIdx = (widget->getTag() == 1) ? 1 : 2;
    
    bool absoluteSaturation = widget->m_hsv.absoluteSaturation;
    float h = widget->m_hsv.h;
    bool absoluteBrightness = widget->m_hsv.absoluteBrightness;
    float s = widget->m_hsv.s;
    float v = widget->m_hsv.v;
    
    cocos2d::CCArray* targets = m_object ? cocos2d::CCArray::createWithObject(m_object) : m_objects;
    
    if (targets) {
        for (int i = 0; i < targets->count(); ++i) {
            auto obj = targets->objectAtIndex(i);
            auto gameObj = static_cast<GameObject*>(obj);
            
            int objType = gameObj->m_objectID;
            if (objType == 1006 || objType == 899) {
                if (colorIdx == 1) {
                    gameObj->m_hue = h;
                    gameObj->m_absoluteSaturation = absoluteSaturation;
                    gameObj->m_saturation = s;
                    gameObj->m_absoluteBrightness = absoluteBrightness;
                    gameObj->m_brightness = v;
                }
            } else {
                auto color = GameObject::getRelativeSpriteColor(gameObj, colorIdx);
                if (color) {
                    color->m_hsv.absoluteSaturation = absoluteSaturation;
                    color->m_hsv.absoluteBrightness = absoluteBrightness;
                    color->m_hsv.h = h;
                    color->m_hsv.s = s;
                    color->m_hsv.v = v;
                }
            }
        }
    }
}

void HSVLiveOverlay::closeColorSelect(cocos2d::CCObject* sender) {
    m_unkBool3 = (sender != nullptr);
    setKeypadEnabled(false);
    
    if (m_delegate) {
        m_delegate->colorSelectClosed(this);
    }
    
    removeFromParentAndCleanup(true);
}

void HSVLiveOverlay::keyBackClicked() {
    closeColorSelect(nullptr);
}

void HSVLiveOverlay::onSelectTab(cocos2d::CCObject* sender) {
    int tab = sender->getTag();
    if (tab != m_activeTab) {
        createHSVWidget(tab);
    }
}

void HSVLiveOverlay::show() {
    auto scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    
    if (!m_scene) {
        m_scene = scene;
    }
    
    if (m_ZOrder == 0) {
        m_ZOrder = 105;
    }
    
    m_scene->addChild(this, m_ZOrder);
}

void HSVLiveOverlay::toggleControls(bool visible) {
    for (int i = 0; i < m_controls->count(); ++i) {
        auto obj = m_controls->objectAtIndex(i);
        obj->setVisible(visible);
    }
}

HSVLiveOverlay::~HSVLiveOverlay() {
    if (m_object) {
        m_object->release();
    }
    if (m_objects) {
        m_objects->release();
    }
    if (m_unkArray) {
        m_unkArray->release();
    }
    if (m_controls) {
        m_controls->release();
    }
}