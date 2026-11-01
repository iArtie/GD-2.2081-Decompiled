#include "FMODLevelVisualizer.h"

FMODLevelVisualizer* FMODLevelVisualizer::create()
{
    FMODLevelVisualizer* visualizer = new FMODLevelVisualizer();
    if (visualizer && visualizer->init())
    {
        visualizer->autorelease();
        return visualizer;
    }
    
    delete visualizer;
    return nullptr;
}

bool FMODLevelVisualizer::init()
{
    if (!cocos2d::CCNode::init())
    {
        return false;
    }
    
    setContentSize(cocos2d::CCSize(7.0f, 100.0f));
    
    cocos2d::CCTextureCache* cache = cocos2d::CCTextureCache::sharedTextureCache();
    cocos2d::CCTexture2D* texture = cache->addImage("PixelSheet_01.png");
    texture->setAliasTexParameters();
    
    cocos2d::CCSpriteBatchNode* batch = cocos2d::CCSpriteBatchNode::createWithTexture(texture, 5);
    addChild(batch);
    batch->setTag(50);
    
    cocos2d::CCSprite* base = cocos2d::CCSprite::createWithSpriteFrameName("pixelart_base_001.png");
    batch->addChild(base, -1);
    base->setColor(cocos2d::ccc3(55, 55, 55));
    base->setScaleX(10.0f / base->getContentSize().width);
    base->setScaleY(100.0f / base->getContentSize().width);
    
    float width = base->getContentSize().width;
    m_width = width;
    float scale = 6.0f / width;
    
    cocos2d::CCPoint pos(-3.0f, -48.0f);
    
    for (int i = 0; i < 3; i++)
    {
        cocos2d::CCPoint p = pos;
        
        int tag;
        float start, end;
        cocos2d::ccColor3B color;
        int opacity;
        
        if (i == 1)
        {
            p.y += 57.6f;
            color = cocos2d::ccc3(255, 200, 0);
            tag = 200;
            end = 0.85f;
            start = 0.6f;
            opacity = 255;
        }
        else if (i == 2)
        {
            p.y += 81.6f;
            color = cocos2d::ccc3(255, 0, 0);
            tag = 300;
            end = 1.0f;
            start = 0.85f;
            opacity = 255;
        }
        else
        {
            p.y += 0.0f;
            color = cocos2d::ccc3(255, 255, 255);
            tag = 100;
            end = 0.6f;
            start = 0.0f;
            opacity = 100;
        }
        
        for (int j = 0; j < 2; j++)
        {
            cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("pixelart_base_001.png");
            batch->addChild(sprite, j);
            sprite->setColor(color);
            if (j == 0)
            {
                sprite->setOpacity(opacity);
            }
            sprite->setPosition(p);
            sprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
            sprite->setScaleX(scale);
            sprite->setScaleY((end - start) * 96.0f / width);
        }
        
        sprite->setTag(tag);
    }
    
    cocos2d::CCSprite* peakSprite = cocos2d::CCSprite::createWithSpriteFrameName("pixelart_base_001.png");
    batch->addChild(peakSprite, 2);
    peakSprite->setColor(cocos2d::ccc3(55, 55, 55));
    peakSprite->setPosition(pos);
    peakSprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    peakSprite->setScaleX(scale);
    peakSprite->setScaleY(1.0f / width);
    peakSprite->setTag(400);
    
    updateVisualizer(0.0f, 0.0f, 0.0f);
    
    return true;
}

void FMODLevelVisualizer::updateVisualizer(float volume, float peak, float time)
{
    cocos2d::CCNode* batch = getChildByTag(50);
    
    for (int i = 0; i < 3; i++)
    {
        int tag = (i == 0) ? 100 : (i == 1) ? 200 : 300;
        cocos2d::CCNode* node = batch->getChildByTag(tag);
        if (!node) continue;
        
        float start, end;
        if (i == 1)
        {
            start = 0.6f;
            end = 0.85f;
        }
        else if (i == 2)
        {
            start = 0.85f;
            end = 1.0f;
        }
        else
        {
            start = 0.0f;
            end = 0.6f;
        }
        
        if (volume > start)
        {
            node->setVisible(true);
            float clamped = (volume > end) ? end : volume;
            node->setScaleY((clamped - start) * 96.0f / m_width);
        }
        else
        {
            node->setVisible(false);
        }
    }
    
    cocos2d::CCNode* peakNode = batch->getChildByTag(400);
    if (peakNode)
    {
        peakNode->setPositionY(peak * 96.0f - 48.0f);
        
        if (peak > 0.85f)
        {
            peakNode->setColor(cocos2d::ccc3(255, 0, 0));
        }
        else if (peak > 0.6f)
        {
            peakNode->setColor(cocos2d::ccc3(255, 255, 0));
        }
        else
        {
            peakNode->setColor(cocos2d::ccc3(0, 255, 0));
        }
        
        int opacity = (int)((0.5f - time) * 255.0f);
        if (opacity > 0)
        {
            peakNode->setOpacity(opacity);
        }
        else
        {
            peakNode->setOpacity(0);
        }
        
        peakNode->setVisible(peak > 0.0f);
    }
}

FMODLevelVisualizer::~FMODLevelVisualizer()
{
}