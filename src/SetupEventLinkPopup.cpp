#include "SetupEventLinkPopup.h"
#include "SetupTriggerPopup.h"
#include "EventLinkTrigger.h"
#include "EffectGameObject.h"
#include "SelectEventLayer.h"
#include "CCMenuItemSpriteExtra.h"

SetupEventLinkPopup* SetupEventLinkPopup::create(EventLinkTrigger* object, cocos2d::CCArray* objects)
{
    SetupEventLinkPopup* instance = new SetupEventLinkPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupEventLinkPopup::init(EventLinkTrigger* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 380.0f, 280.0f, 1))
        return false;

    preSetup();

    addTitle("Event Link");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Activates <cg>Group ID</c> upon a specified player action. Select one or multiple from the top right menu.\n"
        "<cy>Extra ID</c> makes the action only apply if the player interacts with a corresponding object, for example if M"
        "aterial ID matches.\n"
        "<cy>Extra ID2:</c> 0 = All, 1 = P1, 2 = P2",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint groupIDPos = center + cocos2d::CCPoint(-80.0f, 40.0f);
    createValueControlAdvanced(
        51, "Group ID:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &groupIDPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint extraIDPos = center + cocos2d::CCPoint(80.0f, 40.0f);
    createValueControlAdvanced(
        447, "Extra ID:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &extraIDPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint extraID2Pos = center + cocos2d::CCPoint(80.0f, -40.0f);
    createValueControlAdvanced(
        525, "Extra ID2:", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &extraID2Pos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCSprite* selectSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
    selectSprite->setScale(0.55f);
    CCMenuItemSpriteExtra* selectItem = CCMenuItemSpriteExtra::create(
        selectSprite, nullptr, menu_selector(SetupEventLinkPopup::onSelectEvent));
    m_buttonMenu->addChild(selectItem);
    selectItem->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(m_width * 0.5f - 22.0f, m_height * 0.5f - 22.0f)));

    postSetup();

    return true;
}

void SetupEventLinkPopup::onSelectEvent(cocos2d::CCObject* sender)
{
    std::set<int> eventIDs;

    if (m_gameObject)
    {
        eventIDs = m_gameObject->m_eventIDs;
    }
    else if (m_gameObjects && m_gameObjects->count() > 0)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(0);
        eventIDs = obj->m_eventIDs;
    }

    SelectEventLayer* layer = SelectEventLayer::create(this, &eventIDs);
    layer->show();
}

void SetupEventLinkPopup::updateEventIDs(std::set<int>* eventIDs)
{
    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_eventIDs = *eventIDs;
        }
    }
}

SetupEventLinkPopup::~SetupEventLinkPopup()
{
}