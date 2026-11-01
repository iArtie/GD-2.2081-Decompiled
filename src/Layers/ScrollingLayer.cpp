#include "Layers/ScrollingLayer.h"
#include "Utils/GameToolbox.h"

ScrollingLayer* ScrollingLayer::create(cocos2d::CCSize size, cocos2d::CCPoint position, float factor)
{
    ScrollingLayer* returnVal = new ScrollingLayer();

    if (returnVal->init(size, position, factor))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool ScrollingLayer::init(cocos2d::CCSize size, cocos2d::CCPoint position, float factor)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    m_scrollFactor = factor;
    m_position = position;

    cocos2d::CCLayerColor* background = cocos2d::CCLayerColor::create(cocos2d::ccc4(0, 0, 0, 50));
    addChild(background, -2);

    m_contentLayer = cocos2d::CCLayer::create();
    addChild(m_contentLayer);

    m_size = size;

    return true;
}

bool ScrollingLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (!isVisible())
        return false;

    cocos2d::CCRect viewRect = getViewRect();
    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);

    if (m_touchID != -1 || !viewRect.containsPoint(location))
        return false;

    m_touchID = touch->m_nId;
    m_touchStartPosition = location;
    m_touchPosition = location;
    return true;
}

void ScrollingLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    ccTouchEnded(touch, event);
}

void ScrollingLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_touchID == touch->m_nId)
        m_touchID = -1;
}

void ScrollingLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);

    if (location.equals(m_touchPosition))
        return;

    float deltaY = location.y - m_touchPosition.y;
    cocos2d::CCPoint contentPos = m_contentLayer->getPosition();
    contentPos.y += deltaY;

    float minY = m_startOffset.y;
    float maxY = minY + m_scrollFactor;

    if (contentPos.y < minY)
        contentPos.y = minY;
    else if (contentPos.y > maxY)
        contentPos.y = maxY;

    m_contentLayer->setPosition(contentPos);
    m_touchPosition = location;
}

cocos2d::CCRect ScrollingLayer::getViewRect()
{
    cocos2d::CCPoint worldPos = convertToWorldSpace(cocos2d::CCPoint(0.0f, 0.0f));

    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (m_parentLayer)
    {
        scaleX = m_parentLayer->getScaleX();
        scaleY = m_parentLayer->getScaleY();
    }

    return cocos2d::CCRect(
        worldPos.x + scaleX * m_position.x - scaleX * (m_size.width * 0.5f),
        worldPos.y + scaleY * m_position.y - scaleY * (m_size.height * 0.5f),
        scaleX * m_size.width,
        scaleY * m_size.height);
}

void ScrollingLayer::draw()
{
    cocos2d::ccDrawColor4B(0, 20, 40, 255);
    glLineWidth(1.0f);

    cocos2d::CCPoint origin(
        m_position.x - m_size.width * 0.5f,
        m_position.y - m_size.height * 0.5f);

    cocos2d::CCPoint size(m_size.width, m_size.height);
    cocos2d::ccDrawRect(origin, origin + size);
}

void ScrollingLayer::setStartOffset(cocos2d::CCPoint offset)
{
    m_startOffset = offset;
    m_contentLayer->setPosition(offset);
}

void ScrollingLayer::visit()
{
    cocos2d::CCRect viewRect = getViewRect();
    GameToolbox::preVisitWithClipRect(viewRect);
    cocos2d::CCNode::visit();
    GameToolbox::postClipVisit();
}

ScrollingLayer::~ScrollingLayer()
{
}