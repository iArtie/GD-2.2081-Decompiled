#include "BoomScrollLayer.h"
#include "ExtendedLayer.h"
#include "BoomScrollLayerDelegate.h"
#include "DynamicScrollDelegate.h"
#include "GameToolbox.h"
#include "cocos2d.h"
#include <cmath>
#include <ctime>

BoomScrollLayer* BoomScrollLayer::create(cocos2d::CCArray* pages, int offset, bool looped) {
    return create(pages, offset, looped, false, nullptr);
}

BoomScrollLayer* BoomScrollLayer::create(cocos2d::CCArray* pages, int offset, bool looped, bool dynamic, cocos2d::CCArray* dynamicObjects) {
    BoomScrollLayer* layer = new BoomScrollLayer();
    if (layer) {
        // Initialize with parameters
        layer->m_pages = pages;
        if (pages) {
            pages->retain();
        }
        layer->m_pageOffset = offset;
        layer->m_looped = looped;
        layer->m_dynamic = dynamic;
        if (dynamicObjects) {
            dynamicObjects->retain();
        }
        layer->m_dynamicObjects = dynamicObjects;
        layer->autorelease();
        layer->updatePages();
        layer->updateDots(0.0f);
    }
    return layer;
}

void BoomScrollLayer::addPage(cocos2d::CCLayer* page) {
    int count = m_pages->count();
    addPage(page, count);
}

void BoomScrollLayer::addPage(cocos2d::CCLayer* page, int index) {
    m_pages->insertObject(page, index);
    updatePages();
}

void BoomScrollLayer::cancelAndStoleTouch(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    cocos2d::CCSet* set = cocos2d::CCSet::create();
    set->addObject(touch);
    set->autorelease();
    
    m_ignoreTouchCancel = true;
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->getTouchDispatcher()->touchesCancelled(set, event);
    m_ignoreTouchCancel = false;
    
    claimTouch(touch);
}

bool BoomScrollLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_doVisit) {
        return false;
    }
    
    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);
    
    if (m_rect.containsPoint(location) && !m_touch) {
        m_touch = touch;
        
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        
        m_touchType = 0;
        m_touchX = location.x;
        m_touchTime = static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1000000000.0;
        
        return true;
    }
    
    return false;
}

void BoomScrollLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (!m_ignoreTouchCancel && m_touch == touch) {
        m_touch = nullptr;
        selectPage(m_page);
    }
}

void BoomScrollLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touch != touch) {
        return;
    }
    
    m_touch = nullptr;
    
    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);
    
    float deltaX = location.x - m_touchX;
    
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double currentTime = static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1000000000.0;
    double deltaTime = currentTime - m_touchTime;
    
    float speed = fabsf(deltaX) / (deltaTime > 0.0 ? static_cast<float>(deltaTime) : 0.001f);
    m_touchQuotient = speed;
    
    bool shouldChangePage = fabsf(deltaX) >= m_minimumTouchLengthToChangePage;
    bool shouldChangePageBySpeed = speed >= m_unkFloat1 && fabsf(deltaX) >= m_minimumTouchLengthToChangePage * 0.25f;
    
    if (!shouldChangePage && !shouldChangePageBySpeed) {
        moveToPage(m_page);
        return;
    }
    
    cocos2d::CCPoint pos = m_extendedLayer->getPosition();
    int currentPage = pageNumberForPosition(pos);
    
    if (currentPage != m_page) {
        m_page = currentPage;
    }
    
    if (deltaX >= 0.0f) {
        if (m_looped || m_page > 0) {
            m_page--;
        }
    } else {
        if (m_looped || m_page < getTotalPages() - 1) {
            m_page++;
        }
    }
    
    moveToPage(m_page);
}

void BoomScrollLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touch != touch || m_doVisit) {
        return;
    }
    
    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);
    
    if (m_touchType != 1) {
        if (fabsf(location.x - m_touchX) >= m_minimumTouchLengthToSlide) {
            m_extendedLayer->stopActionByTag(2);
            m_pageMoving = false;
            m_touchType = 1;
            m_touchX = location.x;
            
            if (m_cancelAndStealTouch) {
                cancelAndStoleTouch(m_touch, event);
            }
            
            if (m_delegate) {
                m_delegate->scrollLayerScrollingStarted(this);
            }
        }
    }
    
    if (m_touchType == 1) {
        float pageOffset = static_cast<float>(-m_page);
        float pageWidth = m_width - m_pageOffset;
        float newX = location.x + (pageOffset * pageWidth) - m_touchX;
        
        cocos2d::CCPoint pos = m_extendedLayer->getPosition();
        pos.x = newX;
        m_extendedLayer->setPosition(pos);
    }
}

void BoomScrollLayer::claimTouch(cocos2d::CCTouch* touch) {
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    cocos2d::CCTargetedTouchHandler* handler = static_cast<cocos2d::CCTargetedTouchHandler*>(
        dispatcher->findHandler(this)
    );
    
    if (!handler->getClaimedTouches()->containsObject(touch)) {
        handler->getClaimedTouches()->addObject(touch);
    }
}

cocos2d::CCLayer* BoomScrollLayer::getPage(int page) {
    if (!m_dynamic) {
        return static_cast<cocos2d::CCLayer*>(m_pages->objectAtIndex(page));
    }
    
    int count = m_pages->count();
    if (page > 0) {
        page %= count;
    } else if (page == 0) {
        // Keep as 0
    } else {
        while (page < 0) {
            page += count;
        }
    }
    
    return static_cast<cocos2d::CCLayer*>(m_pages->objectAtIndex(page));
}

int BoomScrollLayer::getRelativePageForNum(int page) {
    int totalPages = getTotalPages();
    if (page > 0) {
        return page % totalPages;
    }
    if (page == 0) {
        return 0;
    }
    int result = page;
    while (result < 0) {
        result += totalPages;
    }
    return result;
}

cocos2d::CCPoint BoomScrollLayer::getRelativePosForPage(int page) {
    float width = m_width - m_pageOffset;
    return cocos2d::CCPoint(static_cast<float>(page) * width, 0.0f);
}

int BoomScrollLayer::getTotalPages() {
    if (m_dynamic) {
        return m_dynamicObjects->count();
    }
    return m_pages->count();
}

void BoomScrollLayer::instantMoveToPage(int page) {
    if (m_looped || (page >= 0 && page < getTotalPages())) {
        m_extendedLayer->stopActionByTag(2);
        m_pageMoving = false;
        
        cocos2d::CCPoint pos = positionForPageWithNumber(page);
        m_extendedLayer->setPosition(pos);
        m_page = page;
        
        if (m_looped) {
            repositionPagesLooped();
        }
        
        moveToPageEnded();
    }
}

void BoomScrollLayer::moveToPage(int page) {
    if (!m_looped && (page < 0 || page >= getTotalPages())) {
        return;
    }
    
    m_pageMoving = true;
    m_position = positionForPageWithNumber(page);
    m_extendedLayer->stopActionByTag(2);
    
    float duration = 0.48f;
    if (m_touchQuotient <= m_minSpeed) {
        duration = 0.96f;
    } else if (m_touchQuotient <= m_maxSpeed) {
        duration = 0.72f;
    }
    
    if (m_unkFloat2 > 0.0f) {
        duration *= m_unkFloat2;
    }
    
    m_touchQuotient = 0.0f;
    
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(duration, m_position);
    cocos2d::CCEaseElasticOut* ease = cocos2d::CCEaseElasticOut::create(move, 0.5f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(BoomScrollLayer::moveToPageEnded));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(ease, callFunc, nullptr);
    sequence->setTag(2);
    m_extendedLayer->runAction(sequence);
    
    m_page = page;
    
    if (m_delegate) {
        m_delegate->scrollLayerWillScrollToPage(this, page);
    }
    
    if (m_looped) {
        repositionPagesLooped();
    }
}

void BoomScrollLayer::moveToPageEnded() {
    if (m_slowPage != m_page) {
        if (m_delegate) {
            m_delegate->scrollLayerScrolledToPage(this);
        }
    }
    
    m_extendedLayer->stopActionByTag(2);
    
    cocos2d::CCPoint pos = m_extendedLayer->getPosition();
    int page = pageNumberForPosition(pos);
    m_page = page;
    m_slowPage = page;
    
    updateDots(0.0f);
}

int BoomScrollLayer::pageNumberForPosition(cocos2d::CCPoint pos) {
    float width = m_width - m_pageOffset;
    if (width <= 0.0f) {
        return 0;
    }
    int page = static_cast<int>(-pos.x / width + 0.5f);
    int totalPages = getTotalPages();
    if (page < 0) {
        return 0;
    }
    if (page >= totalPages) {
        return totalPages - 1;
    }
    return page;
}

cocos2d::CCPoint BoomScrollLayer::positionForPageWithNumber(int page) {
    float width = m_width - m_pageOffset;
    return cocos2d::CCPoint(static_cast<float>(-page) * width, 0.0f);
}

void BoomScrollLayer::quickUpdate() {
    if (m_pageMoving) {
        m_pageMoving = false;
        m_extendedLayer->stopActionByTag(2);
        m_extendedLayer->setPosition(m_position);
        moveToPageEnded();
    }
}

void BoomScrollLayer::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -138, false);
}

void BoomScrollLayer::removePage(cocos2d::CCLayer* page) {
    m_pages->removeObject(page);
    this->removeChild(page, true);
    updatePages();
    m_slowPage = m_page;
    
    int count = m_pages->count();
    if (m_page >= count) {
        m_page = count - 1;
    }
    
    moveToPage(m_page);
}

void BoomScrollLayer::removePageWithNumber(int page) {
    if (page >= 0 && page < m_pages->count()) {
        cocos2d::CCLayer* layer = static_cast<cocos2d::CCLayer*>(m_pages->objectAtIndex(page));
        removePage(layer);
    }
}

void BoomScrollLayer::repositionPagesLooped() {
    int currentPage = getRelativePageForNum(m_page);
    int prevPage = getRelativePageForNum(m_page - 1);
    int nextPage = getRelativePageForNum(m_page + 1);
    
    cocos2d::CCLayer* currentLayer = getPage(m_page);
    currentLayer->setPosition(getRelativePosForPage(m_page));
    
    cocos2d::CCLayer* prevLayer = getPage(m_page - 1);
    prevLayer->setPosition(getRelativePosForPage(m_page - 1));
    
    cocos2d::CCLayer* nextLayer = getPage(m_page + 1);
    nextLayer->setPosition(getRelativePosForPage(m_page + 1));
    
    // Hide all pages
    for (int i = 0; i < m_pages->count(); i++) {
        cocos2d::CCLayer* layer = static_cast<cocos2d::CCLayer*>(m_pages->objectAtIndex(i));
        layer->setVisible(false);
    }
    
    if (m_dynamic) {
        cocos2d::CCLayer* pageCurrent = getPage(currentPage);
        cocos2d::CCLayer* pagePrev = getPage(prevPage);
        cocos2d::CCLayer* pageNext = getPage(nextPage);
        
        m_dynamicDelegate->updatePageWithObject(pageCurrent, m_dynamicObjects->objectAtIndex(currentPage));
        m_dynamicDelegate->updatePageWithObject(pagePrev, m_dynamicObjects->objectAtIndex(prevPage));
        m_dynamicDelegate->updatePageWithObject(pageNext, m_dynamicObjects->objectAtIndex(nextPage));
    }
    
    currentLayer->setVisible(true);
    prevLayer->setVisible(!m_pagesInvisible);
    nextLayer->setVisible(!m_pagesInvisible);
}

void BoomScrollLayer::selectPage(int page) {
    if (page >= 0 && page < getTotalPages()) {
        cocos2d::CCPoint pos = positionForPageWithNumber(page);
        m_extendedLayer->setPosition(pos);
        m_slowPage = m_page;
        m_page = page;
    }
}

void BoomScrollLayer::setDotScale(float scale) {
    for (int i = 0; i < m_dots->count(); i++) {
        cocos2d::CCObject* obj = m_dots->objectAtIndex(i);
        obj->setScale(scale);
    }
}

void BoomScrollLayer::setPagesIndicatorPosition(cocos2d::CCPoint pos) {
    m_dotPosition = pos;
    updateDots(0.0f);
}

void BoomScrollLayer::togglePageIndicators(bool visible) {
    m_dotsVisible = visible;
    for (int i = 0; i < m_dots->count(); i++) {
        cocos2d::CCObject* obj = m_dots->objectAtIndex(i);
        obj->setVisible(visible);
    }
}

void BoomScrollLayer::updateDots(float dt) {
    if (!m_dots) {
        return;
    }
    
    int totalPages = getTotalPages();
    cocos2d::CCPoint pos = m_extendedLayer->getPosition();
    int currentPage = pageNumberForPosition(pos);
    
    if (m_looped) {
        currentPage = getRelativePageForNum(currentPage);
    }
    
    for (int i = 0; i < totalPages && i < m_dots->count(); i++) {
        cocos2d::CCObject* dot = m_dots->objectAtIndex(i);
        
        float x = m_dotPosition.x + (static_cast<float>(i) - (static_cast<float>(totalPages - 1) * 0.5f)) * 16.0f;
        float y = m_dotPosition.y;
        dot->setPosition(cocos2d::CCPoint(x, y));
        
        if (i == currentPage) {
            dot->setColor(cocos2d::ccc3(255, 255, 255));
        } else {
            dot->setColor(cocos2d::ccc3(125, 125, 125));
        }
        
        dot->setVisible(m_dotsVisible);
    }
}

void BoomScrollLayer::updatePages() {
    for (int i = 0; i < m_pages->count(); i++) {
        cocos2d::CCNode* node = static_cast<cocos2d::CCNode*>(m_pages->objectAtIndex(i));
        node->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
        
        cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        node->setContentSize(winSize);
        
        float x = static_cast<float>(i) * (m_width - m_pageOffset);
        node->setPosition(cocos2d::CCPoint(x, 0.0f));
        
        if (!node->getParent()) {
            m_extendedLayer->addChild(node);
        }
    }
}

void BoomScrollLayer::visit() {
    if (m_doVisit) {
        GameToolbox::preVisitWithClippingRect(this, m_clippingRect);
    }
    
    cocos2d::CCNode::visit();
    
    if (m_doVisit) {
        GameToolbox::postClipVisit();
    }
}

BoomScrollLayer::~BoomScrollLayer() {
    CC_SAFE_RELEASE_NULL(m_pages);
    CC_SAFE_RELEASE_NULL(m_dynamicObjects);
    CC_SAFE_RELEASE_NULL(m_dots);
    CC_SAFE_RELEASE_NULL(m_extendedLayer);
}