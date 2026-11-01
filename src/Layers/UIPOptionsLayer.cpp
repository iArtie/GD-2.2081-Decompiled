#include "Layers/UIPOptionsLayer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Layers/PlayLayer.h"
#include "Layers/UILayer.h"

bool UIPOptionsLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCPoint location = touch->getLocation();
    cocos2d::CCPoint nodeSpace = convertToNodeSpace(location);

    if (m_touchID < 0)
    {
        cocos2d::CCRect rect = getTouchRect();
        if (rect.containsPoint(nodeSpace))
        {
            m_touchID = touch->m_nId;
            m_touchPosition = nodeSpace;
            m_touchDelta = nodeSpace - m_practiceNode->getPosition();
        }
    }
    return true;
}

void UIPOptionsLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    ccTouchEnded(touch, event);
}

void UIPOptionsLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_touchID == touch->m_nId)
        m_touchID = -1;
}

void UIPOptionsLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCPoint location = touch->getLocation();
    cocos2d::CCPoint nodeSpace = convertToNodeSpace(location);

    if (m_touchID == touch->m_nId)
    {
        const cocos2d::CCPoint& reference = (cocos2d::ccpDistance(nodeSpace, m_touchPosition) > 1.0f)
            ? nodeSpace
            : m_touchPosition;
        cocos2d::CCPoint newPos = reference - m_touchDelta;
        m_practiceNode->setPosition(newPos);
    }
}

UIPOptionsLayer* UIPOptionsLayer::create()
{
    UIPOptionsLayer* layer = new UIPOptionsLayer();

    if (layer->init())
    {
        layer->autorelease();
        return layer;
    }

    delete layer;
    return nullptr;
}

cocos2d::CCRect UIPOptionsLayer::getTouchRect()
{
    cocos2d::CCPoint pos = m_practiceNode->getPosition();
    return cocos2d::CCRect(pos.x - 70.0f, pos.y - 25.0f, 140.0f, 50.0f);
}

float UIPOptionsLayer::getValue(int tag)
{
    if (tag == -29)
        return GameManager::sharedState()->m_practiceOpacity;
    return 0.0f;
}

bool UIPOptionsLayer::init()
{
    if (!SetupTriggerPopup::init(nullptr, nullptr, 400.0f, 280.0f, 1))
        return false;

    preSetup();

    setColor(cocos2d::ccc3(51, 51, 51));
    setOpacity(255);
    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCNode* okSprite = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okButton = CCMenuItemSpriteExtra::create(okSprite, nullptr, menu_selector(UIPOptionsLayer::onClose));
    m_buttonMenu->addChild(okButton);
    cocos2d::CCPoint okPos(winSize.width * 0.5f - 40.0f, winSize.height - 30.0f);
    okButton->setPosition(m_buttonMenu->convertToNodeSpace(okPos));

    cocos2d::CCNode* resetSprite = ButtonSprite::create("Reset", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_05.png", 30.0f);
    CCMenuItemSpriteExtra* resetButton = CCMenuItemSpriteExtra::create(resetSprite, nullptr, menu_selector(UIPOptionsLayer::onReset));
    m_buttonMenu->addChild(resetButton);
    resetButton->setPosition(okButton->getPosition() + cocos2d::CCPoint(80.0f, 0.0f));

    toggleBG(false);

    cocos2d::CCPoint controlBase(winSize.width * 0.5f, winSize.height - 70.0f);
    createValueControlAdvanced(
        -29,
        "Opacity:",
        cocos2d::CCPoint(0.8f, 0.0f),
        1.0f,
        controlBase + cocos2d::CCPoint(0.0f, -75.0f),
        Float,
        1,
        4,
        false,
        false,
        0,
        0,
        GoldLabel,
        1,
        2);

    float opacityValue = GameManager::sharedState()->m_practiceOpacity * 255.0f;

    m_practiceNode = cocos2d::CCNode::create();
    m_practiceNode->setContentSize(cocos2d::CCSize(0, 0));
    m_mainLayer->addChild(m_practiceNode, 9);

    cocos2d::CCSprite* checkpointSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkpointBtn_001.png");
    checkpointSprite->setOpacity((unsigned int)opacityValue);
    m_practiceNode->addChild(checkpointSprite);

    cocos2d::CCSprite* removeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_removeCheckBtn_001.png");
    removeSprite->setOpacity((unsigned int)opacityValue);
    m_practiceNode->addChild(removeSprite);

    cocos2d::CCArray* sprites = cocos2d::CCArray::create();
    sprites->addObject(checkpointSprite);
    sprites->addObject(removeSprite);

    GameManager* gameManager = GameManager::sharedState();
    cocos2d::CCPoint practicePos = gameManager->m_practicePos;
    if (practicePos.x == 0.0f && practicePos.y == 0.0f)
    {
        float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
        practicePos = cocos2d::CCPoint(winSize.width * 0.5f, screenBottom + 40.0f);
    }
    m_practiceNode->setPosition(practicePos);

    GameToolbox::alignItemsHorisontally(sprites, 10.0f, cocos2d::CCPoint(0, 0), false);

    postSetup();

    return true;
}

void UIPOptionsLayer::onClose(cocos2d::CCObject* sender)
{
    GameManager* gameManager = GameManager::sharedState();
    gameManager->m_practicePos = m_practiceNode->getPosition();

    if (GameManager::sharedState()->m_playLayer)
    {
        GameManager::sharedState()->m_playLayer->m_uiLayer->updateCheckState();
    }

    SetupTriggerPopup::onClose(sender);
}

void UIPOptionsLayer::onReset(cocos2d::CCObject* sender)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();

    GameManager* gameManager = GameManager::sharedState();
    gameManager->m_practicePos = cocos2d::CCPoint(winSize.width * 0.5f, screenBottom + 40.0f);

    m_practiceNode->setPosition(GameManager::sharedState()->m_practicePos);

    valueDidChange(-29, 1.0f);
}

void UIPOptionsLayer::registerWithTouchDispatcher()
{
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void UIPOptionsLayer::valueDidChange(int tag, float value)
{
    if (tag == -29)
    {
        GameManager::sharedState()->m_practiceOpacity = value;

        unsigned int index = 0;
        while (index < m_practiceNode->getChildren()->count())
        {
            cocos2d::CCObject* child = m_practiceNode->getChildren()->objectAtIndex(index);
            static_cast<cocos2d::CCNode*>(child)->setOpacity((unsigned int)(value * 255.0f));
            index++;
        }
    }
}

UIPOptionsLayer::~UIPOptionsLayer()
{
}