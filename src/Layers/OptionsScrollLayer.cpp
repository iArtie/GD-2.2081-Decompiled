#include "Layers/OptionsScrollLayer.h"
#include "Layers/GJCommentListLayer.h"
#include "UI/CustomListView.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Utils/GameToolbox.h"

OptionsScrollLayer* OptionsScrollLayer::create(cocos2d::CCArray* objects, bool recreate, int minimum)
{
    OptionsScrollLayer* returnVal = new OptionsScrollLayer();
    if (returnVal->init(objects, recreate, minimum))
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool OptionsScrollLayer::init(cocos2d::CCArray* objects, bool recreate, int minimum)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_recreateList = recreate;
    m_noElasticity = true;
    m_minCount = minimum;

    if (objects)
    {
        m_optionObjects = objects;
        cocos2d::CCObject::retain(objects);
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(OptionsScrollLayer::onClose));
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setSizeMult(1.6f);

    cocos2d::CCPoint closePos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f));
    closeBtn->setPosition(closePos);

    setupList(m_optionObjects);

    return true;
}

cocos2d::CCArray* OptionsScrollLayer::getRelevantObjects(cocos2d::CCArray* objects)
{
    cocos2d::CCArray* returnVal = cocos2d::CCArray::create();
    if (objects)
    {
        for (unsigned int i = 0; i < objects->count(); i++)
        {
            OptionsObject* obj = (OptionsObject*)objects->objectAtIndex(i);
            if (!obj)
                break;
            if (obj->m_count > m_minCount || obj->m_enabled)
                returnVal->addObject(obj);
        }
    }
    return returnVal;
}

void OptionsScrollLayer::keyBackClicked()
{
    onClose(nullptr);
}

void OptionsScrollLayer::onClose(cocos2d::CCObject* sender)
{
    if (m_listLayer)
    {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }

    if (m_alertProtocol)
        m_alertProtocol->FLAlert_Clicked(this, true);

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void OptionsScrollLayer::registerWithTouchDispatcher()
{
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void OptionsScrollLayer::setupList(cocos2d::CCArray* objects)
{
    if (m_listLayer)
    {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    CustomListView* listView = nullptr;
    if (objects)
    {
        cocos2d::CCArray* relevant = getRelevantObjects(objects);
        listView = CustomListView::create(relevant, BoomListType::Default, 220.0f, 340.0f);
    }

    cocos2d::ccColor4B color = cocos2d::ccc4(60, 60, 60, 255);
    m_listLayer = GJCommentListLayer::create(listView, "", color, 340.0f, 220.0f, false);

    m_mainLayer->addChild(m_listLayer);
    m_listLayer->setPosition(cocos2d::CCPoint(
        (winSize.width - 340.0f) * 0.5f,
        (winSize.height - 220.0f) * 0.5f - 10.0f));
}

OptionsScrollLayer::~OptionsScrollLayer()
{
}