#include "UIOptionsLayer.h"
#include "GJUINode.h"
#include "GameManager.h"
#include "GameToolbox.h"

UIOptionsLayer* UIOptionsLayer::create(bool dual)
{
    UIOptionsLayer* layer = new UIOptionsLayer();

    if (layer->init(dual))
    {
        layer->autorelease();
        return layer;
    }

    delete layer;
    return nullptr;
}

bool UIOptionsLayer::init(bool dual)
{
    if (!FLAlertLayer::init(nullptr, 0, 0, 0, 0, 0, 0, 0, 0, 0))
        return false;

    m_dual = dual;

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu);

    m_nameLabel = cocos2d::CCLabelBMFont::create("", "goldFont.fnt");
    m_mainLayer->addChild(m_nameLabel);

    m_uiNode1 = GJUINode::create(this, 1);
    m_uiNode2 = GJUINode::create(this, 2);
    m_uiNode3 = GJUINode::create(this, 3);
    m_uiNode4 = GJUINode::create(this, 4);

    m_mainLayer->addChild(m_uiNode1);
    m_mainLayer->addChild(m_uiNode2);
    m_mainLayer->addChild(m_uiNode3);
    m_mainLayer->addChild(m_uiNode4);

    m_activeUIGroup = 0;
    toggleUIGroup(0);

    return true;
}

bool UIOptionsLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    return FLAlertLayer::ccTouchBegan(touch, event);
}

void UIOptionsLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    FLAlertLayer::ccTouchCancelled(touch, event);
}

void UIOptionsLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    FLAlertLayer::ccTouchEnded(touch, event);
}

void UIOptionsLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    FLAlertLayer::ccTouchMoved(touch, event);
}

void UIOptionsLayer::registerWithTouchDispatcher()
{
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

GJUINode* UIOptionsLayer::getNode(int type)
{
    switch (type)
    {
        case 1: return m_uiNode1;
        case 2: return m_uiNode2;
        case 3: return m_uiNode3;
        case 4: return m_uiNode4;
        default: return nullptr;
    }
}

float UIOptionsLayer::getValue(int tag)
{
    return GameManager::sharedState()->getUGV(tag);
}

void UIOptionsLayer::onClose(cocos2d::CCObject* sender)
{
    FLAlertLayer::onClose(sender);
}

void UIOptionsLayer::onReset(cocos2d::CCObject* sender)
{
}

void UIOptionsLayer::onSaveLoad(cocos2d::CCObject* sender)
{
}

void UIOptionsLayer::toggleUIGroup(int group)
{
    m_activeUIGroup = group;

    if (m_uiNode1) m_uiNode1->setVisible(group == 1);
    if (m_uiNode2) m_uiNode2->setVisible(group == 2);
    if (m_uiNode3) m_uiNode3->setVisible(group == 3);
    if (m_uiNode4) m_uiNode4->setVisible(group == 4);
}

void UIOptionsLayer::valueDidChange(int tag, float value)
{
    GameManager::sharedState()->setUGV(tag, value);
}

UIOptionsLayer::~UIOptionsLayer()
{
}