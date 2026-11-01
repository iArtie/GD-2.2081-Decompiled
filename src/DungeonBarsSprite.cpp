#include "DungeonBarsSprite.h"

DungeonBarsSprite* DungeonBarsSprite::create()
{
    DungeonBarsSprite* pRet = new DungeonBarsSprite();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DungeonBarsSprite::init()
{
    if (!cocos2d::CCNode::init())
        return false;
    
    m_barsSprite = cocos2d::CCSprite::createWithSpriteFrameName("dungeon_bars_001.png");
    addChild(m_barsSprite);
    m_barsSprite->setPosition(cocos2d::CCPoint(0.0f, -2.0f));
    
    return true;
}

void DungeonBarsSprite::animateOutBars()
{
    // Shake animation sequence
    cocos2d::CCAction* shake = cocos2d::CCSequence::create(
        cocos2d::CCMoveBy::create(0.08f, cocos2d::CCPoint(-1.5f, 0.0f)),
        cocos2d::CCMoveBy::create(0.08f, cocos2d::CCPoint(1.5f, 0.0f)),
        cocos2d::CCMoveBy::create(0.08f, cocos2d::CCPoint(-1.5f, 0.0f)),
        cocos2d::CCMoveBy::create(0.08f, cocos2d::CCPoint(1.5f, 0.0f)),
        cocos2d::CCMoveBy::create(0.08f, cocos2d::CCPoint(-1.5f, 0.0f)),
        cocos2d::CCMoveBy::create(0.08f, cocos2d::CCPoint(1.5f, 0.0f)),
        cocos2d::CCEaseElasticIn::create(
            cocos2d::CCMoveBy::create(2.5f, cocos2d::CCPoint(0.0f, -130.0f)),
            1.6f
        ),
        nullptr
    );
    
    m_barsSprite->runAction(shake);
}

void DungeonBarsSprite::visit()
{
    cocos2d::CCSize size = cocos2d::CCSize(140.0f, 125.0f);
    cocos2d::CCRect rect = cocos2d::CCRect(
        -size.width * 0.5f,
        -size.height * 0.5f,
        size.width,
        size.height
    );
    
    // Calculate scissor rect in world space
    cocos2d::CCPoint anchor = getAnchorPoint();
    float x = rect.origin.x * anchor.x;
    float y = rect.origin.y * anchor.y;
    float w = rect.size.width * getScaleX();
    float h = rect.size.height * getScaleY();
    
    cocos2d::CCRect scissorRect = cocos2d::CCRect(x, y, w, h);
    
    glEnable(GL_SCISSOR_TEST);
    
    cocos2d::CCNode* parent = getParent();
    cocos2d::CCPoint pos = getPosition();
    cocos2d::CCPoint worldPos = parent->convertToWorldSpace(pos + rect.origin);
    
    cocos2d::CCEGLView* view = cocos2d::CCEGLView::sharedOpenGLView();
    view->setScissorInPoints(
        worldPos.x,
        worldPos.y,
        scissorRect.size.width,
        scissorRect.size.height
    );
    
    cocos2d::CCNode::visit();
    
    glDisable(GL_SCISSOR_TEST);
}

DungeonBarsSprite::~DungeonBarsSprite()
{
    // Base class handles cleanup
}