#include "Utils/PromoInterstitial.h"
#include "Layers/FLAlertLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Managers/GameManager.h"
#include "Utils/PlatformToolbox.h"
#include "Utils/AdToolbox.h"
#include "Layers/PlayLayer.h"

bool PromoInterstitial::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected != -1)
        return true;

    if (m_closeMenu && m_closeMenu->ccTouchBegan(touch, event))
    {
        m_joystickConnected = touch->m_nId;
        m_promoTouched = true;
        return true;
    }

    if (!m_buttonMenu || !m_buttonMenu->ccTouchBegan(touch, event))
        return true;

    m_joystickConnected = touch->m_nId;
    m_promoTouched = false;
    return true;
}

void PromoInterstitial::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected != touch->m_nId)
        return;

    if (m_promoTouched)
        m_closeMenu->ccTouchCancelled(touch, event);
    else
        m_buttonMenu->ccTouchCancelled(touch, event);
}

void PromoInterstitial::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected != touch->m_nId)
        return;

    if (m_promoTouched)
        m_closeMenu->ccTouchEnded(touch, event);
    else
        m_buttonMenu->ccTouchEnded(touch, event);

    m_joystickConnected = -1;
}

void PromoInterstitial::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected != touch->m_nId)
        return;

    if (m_promoTouched)
        m_closeMenu->ccTouchMoved(touch, event);
    else
        m_buttonMenu->ccTouchMoved(touch, event);
}

PromoInterstitial* PromoInterstitial::create(bool fullVersion)
{
    PromoInterstitial* returnVal = new PromoInterstitial();

    if (returnVal->init(fullVersion))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool PromoInterstitial::init(bool fullVersion)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_fullVersion = fullVersion;
    setup();
    setVisible(false);
    return true;
}

void PromoInterstitial::keyBackClicked()
{
    setKeypadEnabled(false);
    onClose(nullptr);
}

void PromoInterstitial::onClick(cocos2d::CCObject* sender)
{
    PlatformToolbox::logEvent("gjl_store");
    cocos2d::CCApplication::sharedApplication()->openURL(m_marketLink.c_str());
    onClose(nullptr);
}

void PromoInterstitial::onClose(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_unkBool7 = false;

    if (m_showAdBanner && GameManager::sharedState()->m_playLayer)
        AdToolbox::enableBanner();

    removeFromParentAndCleanup(true);
}

void PromoInterstitial::setup()
{
    if (m_fullVersion)
        m_marketLink = "market://details?id=com.robtopx.geometryjumplite";
    else
        m_marketLink = "market://details?id=com.robtopx.geometryjump";

    m_mainLayer->setPosition(cocos2d::CCPoint(0.0f, 0.0f));

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    const char* promoSpriteName = m_fullVersion ? "GJ_popup.png" : "GJ_popupSZ.png";
    cocos2d::CCSprite* promoSprite = cocos2d::CCSprite::create(promoSpriteName);

    CCMenuItemSpriteExtra* promoItem = CCMenuItemSpriteExtra::create(promoSprite, nullptr, menu_selector(PromoInterstitial::onClick));
    promoItem->m_animationEnabled = false;
    promoItem->m_colorEnabled = true;
    promoItem->setSizeMult(0.9f);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeItem = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(PromoInterstitial::onClose));
    closeItem->setSizeMult(2.0f);

    m_buttonMenu = cocos2d::CCMenu::create(promoItem, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 1);

    m_closeMenu = cocos2d::CCMenu::create(closeItem, nullptr);
    m_mainLayer->addChild(m_closeMenu, 2);

    cocos2d::CCPoint promoPos(winSize.width * 0.5f, winSize.height * 0.5f);
    cocos2d::CCPoint promoWorld = m_mainLayer->convertToWorldSpace(promoPos);
    promoItem->setPosition(m_buttonMenu->convertToNodeSpace(promoWorld));

    cocos2d::CCPoint closeOffset(
        10.0f - promoSprite->getContentSize().height * 0.5f,
        promoSprite->getScaleY() * 0.5f - 10.0f);
    cocos2d::CCPoint closeWorldPos = promoPos + closeOffset;
    cocos2d::CCPoint closeWorld = m_mainLayer->convertToWorldSpace(closeWorldPos);
    closeItem->setPosition(m_closeMenu->convertToNodeSpace(closeWorld));
}

void PromoInterstitial::show()
{
    GameManager::sharedState()->m_unkBool7 = true;
    FLAlertLayer::show();
}

PromoInterstitial::~PromoInterstitial()
{
}