#include "Editor/EditButtonBar.h"
#include "Editor/ButtonPage.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

EditButtonBar* EditButtonBar::create()
{
    EditButtonBar* pRet = new EditButtonBar();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool EditButtonBar::init()
{
    if (!cocos2d::CCNode::init())
        return false;
    
    m_scrollLayer = nullptr;
    m_buttonArray = nullptr;
    m_pagesArray = cocos2d::CCArray::create();
    m_pagesArray->retain();
    m_hasCreateItems = false;
    m_tabIndex = 0;
    m_position = cocos2d::CCPoint(0, 0);
    
    return true;
}

int EditButtonBar::getPage()
{
    if (!m_scrollLayer)
        return 0;
    return BoomScrollLayer::getRelativePageForNum(m_scrollLayer, m_scrollLayer->m_page);
}

void EditButtonBar::goToPage(int page)
{
    if (!m_scrollLayer)
        return;
    
    int totalPages = BoomScrollLayer::getTotalPages(m_scrollLayer);
    if (page < 1)
        page = 1;
    if (page > totalPages)
        page = totalPages;
    
    BoomScrollLayer::instantMoveToPage(m_scrollLayer, page - 1);
    BoomScrollLayer::instantMoveToPage(m_scrollLayer, page);
}

void EditButtonBar::loadFromItems(cocos2d::CCArray* objects, int rows, int columns, bool keepPage)
{
    int currentPage = 0;
    bool shouldKeepPage = false;
    
    if (m_scrollLayer)
    {
        currentPage = m_scrollLayer->m_page;
        shouldKeepPage = keepPage && currentPage > 0;
    }
    
    if (objects)
        objects->retain();
    
    // Clear existing button array
    if (m_buttonArray)
    {
        for (unsigned int i = 0; i < m_buttonArray->count(); i++)
        {
            cocos2d::CCObject* obj = m_buttonArray->objectAtIndex(i);
            // Clean up button objects
        }
        m_buttonArray->release();
    }
    
    // Remove old scroll layer
    if (m_scrollLayer)
    {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_scrollLayer);
        m_scrollLayer->removeMeAndCleanup();
        m_scrollLayer = nullptr;
    }
    
    m_buttonArray = objects;
    m_pagesArray->removeAllObjects();
    removeAllChildren();
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Calculate button scale based on available space
    float spacing = 4.0f;
    float maxWidth = 240.0f;
    float buttonSize = 40.0f;
    
    float totalSpacing = spacing * (columns - 1);
    float availableWidth = maxWidth - totalSpacing;
    float availableHeight = centerY - spacing * (rows - 1);
    
    float scaleX = availableWidth / (columns * buttonSize);
    float scaleY = availableHeight / (rows * buttonSize);
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    
    // Limit scale to reasonable values
    float maxScale = (78.0f - totalSpacing) / (columns * buttonSize);
    if (scale > maxScale)
        scale = maxScale;
    
    int itemsPerPage = rows * columns;
    int pageIndex = 0;
    
    // Distribute items into pages
    for (unsigned int i = 0; i < objects->count(); i++)
    {
        cocos2d::CCObject* obj = objects->objectAtIndex(i);
        
        // Scale the button
        obj->setScale(scale);
        
        // For create items, store page and tab info
        if (m_hasCreateItems)
        {
            obj[7].m_uUnknown = pageIndex;
            obj[7].m_unknown2 = m_tabIndex;
        }
        
        // Get or create current page array
        cocos2d::CCArray* pageArray;
        if (pageIndex >= m_pagesArray->count())
        {
            pageArray = cocos2d::CCArray::create();
            m_pagesArray->addObject(pageArray);
        }
        else
        {
            pageArray = (cocos2d::CCArray*)m_pagesArray->objectAtIndex(pageIndex);
        }
        
        // Add item to page
        pageArray->addObject(obj);
        
        // Move to next page if current page is full
        if (pageArray->count() >= itemsPerPage)
            pageIndex++;
    }
    
    // Create ButtonPages
    float buttonWidth = scale * buttonSize;
    float buttonHeight = scale * buttonSize;
    
    for (unsigned int i = 0; i < m_pagesArray->count(); i++)
    {
        cocos2d::CCArray* pageItems = (cocos2d::CCArray*)m_pagesArray->objectAtIndex(i);
        cocos2d::CCSize pageSize = cocos2d::CCSize(buttonWidth * columns, buttonHeight * rows);
        cocos2d::CCPoint position = m_position;
        
        ButtonPage* page = ButtonPage::create(pageItems, pageSize, position, 0.0f);
        m_pagesArray->replaceObjectAtIndex(i, page);
    }
    
    // Create scroll layer
    bool hasMultiplePages = m_pagesArray->count() > 1;
    BoomScrollLayer* scrollLayer = BoomScrollLayer::create(m_pagesArray, nullptr, hasMultiplePages);
    m_scrollLayer = scrollLayer;
    scrollLayer->m_pagesInvisible = true;
    scrollLayer->setDotScale(0.4f);
    addChild(scrollLayer);
    
    // Position pages indicator
    scrollLayer->setPagesIndicatorPosition(cocos2d::CCPoint(centerX, 2.0f));
    scrollLayer->setTouchEnabled(false);
    
    // Add navigation arrows if multiple pages
    if (hasMultiplePages)
    {
        cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
        addChild(menu);
        
        // Left arrow
        cocos2d::CCSprite* leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        leftSprite->setScale(0.6f);
        CCMenuItemSpriteExtra* leftBtn = CCMenuItemSpriteExtra::create(
            leftSprite, nullptr, this, menu_selector(EditButtonBar::onLeft));
        leftBtn->setSizeMult(1.1f);
        menu->addChild(leftBtn);
        
        float arrowOffset = (centerX - 240.0f) * 0.5f + 16.0f;
        float totalHeight = buttonHeight * rows + spacing * (rows - 1);
        float yPos = m_position.y - totalHeight * 0.5f;
        
        leftBtn->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_position.x - arrowOffset, yPos)));
        
        // Right arrow
        cocos2d::CCSprite* rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        rightSprite->setScale(0.6f);
        rightSprite->setFlipX(true);
        CCMenuItemSpriteExtra* rightBtn = CCMenuItemSpriteExtra::create(
            rightSprite, nullptr, this, menu_selector(EditButtonBar::onRight));
        rightBtn->setSizeMult(1.1f);
        menu->addChild(rightBtn);
        
        rightBtn->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_position.x + arrowOffset, yPos)));
    }
    
    // Restore previous page if requested
    if (shouldKeepPage)
    {
        scrollLayer->instantMoveToPage(currentPage - 1);
        scrollLayer->instantMoveToPage(currentPage);
    }
}

void EditButtonBar::onLeft(cocos2d::CCObject* sender)
{
    if (m_scrollLayer)
        goToPage(m_scrollLayer->m_page - 1);
}

void EditButtonBar::onRight(cocos2d::CCObject* sender)
{
    if (m_scrollLayer)
        goToPage(m_scrollLayer->m_page + 1);
}

void EditButtonBar::reloadItems(int rowCount, int columnCount)
{
    if (m_buttonArray)
        loadFromItems(m_buttonArray, rowCount, columnCount, false);
}

EditButtonBar::~EditButtonBar()
{
    CC_SAFE_RELEASE(m_buttonArray);
    CC_SAFE_RELEASE(m_pagesArray);
    // m_scrollLayer is child node, will be cleaned up by parent
}