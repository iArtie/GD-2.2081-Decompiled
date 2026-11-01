#include "CCScrollLayerExt.h"
#include "CCContentLayer.h"
#include "CCScrollLayerExtDelegate.h"
#include "GameToolbox.h"
#include "cocos2d.h"

CCScrollLayerExt::CCScrollLayerExt(cocos2d::CCRect rect)
{
    m_disableMovement = false;
    m_cutContent = true;
    setTouchEnabled(true);
    m_disableVertical = false;
    m_disableHorizontal = false;
    setContentSize(rect.size);
    setPosition(rect.origin);
    m_vScrollbarVisible = false;
    m_hScrollbarVisible = false;
    m_delegate = nullptr;
    m_touch = nullptr;
    m_peekLimitTop = 50.0f;
    m_peekLimitBottom = 50.0f;
    m_scrollLimitBottom = 0.0f;
    m_scrollLimitTop = 0.0f;
    m_touchPosition = cocos2d::CCPoint(0, 0);
    m_touchStartPosition = cocos2d::CCPoint(0, 0);
    m_touchDown = false;
    m_notAtEndOfScroll = false;

    // Content layer
    m_contentLayer = CCContentLayer::create(cocos2d::ccc4(0, 0, 0, 0), rect.size.width, 0.0f);
    m_contentLayer->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    this->addChild(m_contentLayer);

    // Vertical scrollbar
    float scrollbarHeight = (rect.size.height * rect.size.height) / m_contentLayer->getContentSize().height;
    m_verticalScrollbar = cocos2d::CCLayerColor::create(
        cocos2d::ccc4(255, 255, 255, 180),
        4.0f,
        scrollbarHeight
    );
    m_verticalScrollbar->setPosition(cocos2d::CCPoint(
        rect.size.width - 2.0f,
        rect.size.height - scrollbarHeight * 0.5f
    ));
    m_verticalScrollbar->setTouchEnabled(false);
    this->addChild(m_verticalScrollbar);
    m_verticalScrollbar->setVisible(false);

    // Horizontal scrollbar
    float scrollbarWidth = (rect.size.width * rect.size.width) / m_contentLayer->getContentSize().width;
    m_horizontalScrollbar = cocos2d::CCLayerColor::create(
        cocos2d::ccc4(255, 255, 255, 180),
        scrollbarWidth,
        4.0f
    );
    m_horizontalScrollbar->setPosition(cocos2d::CCPoint(
        scrollbarWidth * 0.5f,
        scrollbarHeight * 0.5f
    ));
    m_horizontalScrollbar->setTouchEnabled(false);
    this->addChild(m_horizontalScrollbar);
    m_horizontalScrollbar->setVisible(false);

    schedule(schedule_selector(CCScrollLayerExt::updateIndicators), 0.0f);
}

bool CCScrollLayerExt::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_disableMovement)
        return false;

    m_touch = touch;
    m_touchPosition = touch->getLocationInView();
    m_touchStartPosition = m_touchPosition;
    cocos2d::CCPoint localPos = this->convertToNodeSpace(m_touchPosition);

    cocos2d::CCRect rect = this->getBoundingBox();
    if (!rect.containsPoint(localPos))
        return false;

    m_contentLayer->stopAllActions();
    gettimeofday(&m_timeValue, nullptr);
    m_touchDown = true;
    return !m_disableMovement;
}

void CCScrollLayerExt::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    constraintContent();
    m_touchPosition = cocos2d::CCPoint(0, 0);
}

void CCScrollLayerExt::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_touch != touch)
        return;

    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);

    struct timeval now;
    gettimeofday(&now, nullptr);
    float distance = ccpDistance(location, m_touchStartPosition);
    float timeDelta = timersub(&m_timeValue, &now);

    if (distance / timeDelta < 0.5f)
    {
        constraintContent();
    }
    else
    {
        m_contentLayer->stopAllActions();
        cocos2d::CCPoint offset = location - m_touchStartPosition;

        if (m_disableVertical)
            offset.y = 0.0f;
        else if (m_disableHorizontal)
            offset.x = 0.0f;

        float minY = getMinY();
        float maxY = getMaxY() + m_peekLimitBottom;
        cocos2d::CCPoint layerPos = m_contentLayer->getPosition();
        float targetY = layerPos.y + offset.y;

        float duration = 0.3f;
        if (targetY > maxY)
        {
            targetY = maxY;
            duration = 0.1f;
        }
        else if (targetY < minY - m_peekLimitTop)
        {
            targetY = minY - m_peekLimitTop;
            duration = 0.1f;
        }

        if (m_delegate)
            m_delegate->scrllViewWillBeginDecelerating(this);

        constraintContent();

        cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(duration, cocos2d::CCPoint(layerPos.x, targetY));
        cocos2d::CCEaseOut* ease = cocos2d::CCEaseOut::create(move, 2.0f);
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(CCScrollLayerExt::constraintContent));
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(ease, callFunc, nullptr);
        m_contentLayer->runAction(sequence);
        m_notAtEndOfScroll = true;
    }

    m_touchDown = false;
    m_touchPosition = cocos2d::CCPoint(0, 0);
}

void CCScrollLayerExt::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    m_touchDown = true;

    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);

    if (m_touch != touch)
        return;

    cocos2d::CCPoint offset = location - m_touchPosition;

    if (m_disableVertical)
        offset.y = 0.0f;
    else if (m_disableHorizontal)
        offset.x = 0.0f;

    cocos2d::CCPoint layerPos = m_contentLayer->getPosition();
    layerPos += offset;

    float minY = getMinY() - m_peekLimitTop;
    float maxY = getMaxY() + m_peekLimitBottom;

    if (layerPos.y < minY)
        layerPos.y = minY;
    else if (layerPos.y > maxY)
        layerPos.y = maxY;

    m_contentLayer->setPosition(layerPos);
    m_notAtEndOfScroll = true;
    m_touchPosition = location;

    if (m_delegate)
        m_delegate->scrollViewTouchMoving(this);
}

void CCScrollLayerExt::constraintContent()
{
    doConstraintContent(false);
}

void CCScrollLayerExt::doConstraintContent(bool instant)
{
    cocos2d::CCPoint pos = m_contentLayer->getPosition();
    float minY = getMinY();
    float maxY = getMaxY();

    if (pos.y >= minY && pos.y <= maxY)
    {
        if (m_delegate)
            m_delegate->scrollViewDidEndMoving(this);
        return;
    }

    m_contentLayer->stopAllActions();

    float targetY = (pos.y > maxY) ? maxY : minY;

    if (instant)
    {
        m_contentLayer->setPosition(cocos2d::CCPoint(pos.x, targetY));
        scrollingEnd();
    }
    else
    {
        if (m_delegate)
            m_delegate->scrllViewWillBeginDecelerating(this);

        cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.3f, cocos2d::CCPoint(pos.x, targetY));
        cocos2d::CCEaseInOut* ease = cocos2d::CCEaseInOut::create(move, 2.0f);
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(CCScrollLayerExt::scrollingEnd));
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(ease, callFunc, nullptr);
        m_contentLayer->runAction(sequence);
    }
}

float CCScrollLayerExt::getMaxY()
{
    return 0.0f;
}

float CCScrollLayerExt::getMinY()
{
    float height = getContentSize().height;
    return height - m_contentLayer->getContentSize().height - m_scrollLimitTop;
}

void CCScrollLayerExt::moveToTop()
{
    float x = m_contentLayer->getPosition().x;
    float minY = getMinY();
    m_contentLayer->setPosition(cocos2d::CCPoint(x, minY));
}

void CCScrollLayerExt::moveToTopWithOffset(float offset)
{
    float x = m_contentLayer->getPosition().x;
    float minY = getMinY();
    m_contentLayer->setPosition(cocos2d::CCPoint(x, minY + offset));
}

void CCScrollLayerExt::postVisit()
{
    glDisable(GL_SCISSOR_TEST);
}

void CCScrollLayerExt::registerWithTouchDispatcher()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->getTouchDispatcher()->addTargetedDelegate(this, -128, false);
}

void CCScrollLayerExt::scrollLayer(float offset)
{
    cocos2d::CCPoint pos = m_contentLayer->getPosition();
    pos.y += offset;

    float minY = getMinY();
    float maxY = getMaxY();

    if (pos.y < minY)
        pos.y = minY;
    else if (pos.y > maxY)
        pos.y = maxY;

    m_contentLayer->setPosition(pos);
}

void CCScrollLayerExt::scrollingEnd()
{
    m_contentLayer->stopAllActions();
    m_notAtEndOfScroll = false;

    if (m_delegate)
        m_delegate->scrollViewDidEndDecelerating(this);
}

void CCScrollLayerExt::updateIndicators(float dt)
{
    cocos2d::CCSize contentSize = m_contentLayer->getContentSize();
    cocos2d::CCSize viewSize = getContentSize();

    // Vertical scrollbar
    if (m_hScrollbarVisible)
    {
        float scrollbarHeight = (viewSize.height * viewSize.height) / contentSize.height;
        m_verticalScrollbar->setContentSize(cocos2d::CCSize(4.0f, scrollbarHeight));

        float posY = m_contentLayer->getPosition().y;
        float offset = ((posY - viewSize.height) * scrollbarHeight) / viewSize.height;
        m_verticalScrollbar->setPosition(cocos2d::CCPoint(
            m_verticalScrollbar->getPosition().x,
            viewSize.height - offset - scrollbarHeight * 0.5f
        ));
        m_verticalScrollbar->setVisible(m_hScrollbarVisible);
    }

    // Horizontal scrollbar
    if (m_vScrollbarVisible)
    {
        float scrollbarWidth = (viewSize.width * viewSize.width) / contentSize.width;
        m_horizontalScrollbar->setContentSize(cocos2d::CCSize(scrollbarWidth, 4.0f));

        float posX = m_contentLayer->getPosition().x;
        float offset = ((posX - viewSize.width) * scrollbarWidth) / viewSize.width;
        m_horizontalScrollbar->setPosition(cocos2d::CCPoint(
            -offset + viewSize.width * 0.5f - scrollbarWidth * 0.5f,
            m_horizontalScrollbar->getPosition().y
        ));
        m_horizontalScrollbar->setVisible(m_vScrollbarVisible);
    }
}

void CCScrollLayerExt::visit()
{
    if (m_cutContent)
    {
        cocos2d::CCPoint pos = getPosition();
        cocos2d::CCSize size = getContentSize();
        float scaleX = getScaleX();
        float scaleY = getScaleY();

        cocos2d::CCRect rect(pos.x, pos.y, size.width * scaleX, size.height * scaleY);
        GameToolbox::preVisitWithClippingRect(this, rect);
    }

    cocos2d::CCNode::visit(this);

    if (m_cutContent)
        GameToolbox::postClipVisit();
}

CCScrollLayerExt::~CCScrollLayerExt()
{
    // Destructor
}