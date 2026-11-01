#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

ButtonSprite* ButtonSprite::create(const char* caption)
{
    ButtonSprite* sprite = new ButtonSprite();
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        sprite->setString(caption);
        return sprite;
    }
    
    delete sprite;
    return nullptr;
}

void ButtonSprite::setString(const char* text)
{
    if (m_mode != 1)
    {
        return;
    }
    
    m_label->setScale(1.0f);
    m_label->setString(text);
    m_caption = text;
    
    if (m_width > 0.0f)
    {
        cocos2d::CCSize labelSize = m_label->getContentSize();
        if (labelSize.width > m_width)
        {
            float scale = m_width / labelSize.width;
            m_label->setScale(scale);
        }
    }
    
    float currentScale = m_label->getScale();
    if (currentScale > m_scale)
    {
        m_label->setScale(m_scale);
    }
    
    float finalWidth;
    if (m_absolute)
    {
        finalWidth = m_width;
    }
    else
    {
        float scaledWidth = m_label->getContentSize().width * m_label->getScale();
        finalWidth = (scaledWidth > m_minWidth) ? scaledWidth : m_minWidth;
    }
    
    if (m_height == 0.0f)
    {
        m_height = m_label->getContentSize().height + 4.0f;
    }
    
    m_BGSprite->setContentSize(cocos2d::CCSize(finalWidth + 16.0f, m_height));
    setContentSize(m_BGSprite->getContentSize());
    
    cocos2d::CCPoint labelPos(
        m_textOffset.x + getContentSize().width * 0.5f,
        m_textOffset.y + getContentSize().height * 0.5f
    );
    m_label->setPosition(labelPos);
    
    cocos2d::CCPoint bgPos(
        getContentSize().width * 0.5f,
        getContentSize().height * 0.5f
    );
    m_BGSprite->setPosition(bgPos);
    
    if (getParent())
    {
        CCMenuItemSpriteExtra* parent = dynamic_cast<CCMenuItemSpriteExtra*>(getParent());
        if (parent)
        {
            parent->setContentSize(getContentSize());
            setPosition(cocos2d::CCPoint(getContentSize().width * 0.5f, getContentSize().height * 0.5f));
        }
    }
}

void ButtonSprite::updateBGImage(const char* filename)
{
    if (m_BGSprite)
    {
        m_BGSprite->removeMeAndCleanup();
        m_BGSprite = cocos2d::extension::CCScale9Sprite::create(filename, cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
        m_BGSprite->setContentSize(cocos2d::CCSize(16.0f, 16.0f));
        addChild(m_BGSprite, 0);
        
        if (m_mode == 1)
        {
            setString(m_caption.c_str());
        }
        else
        {
            updateSpriteBGSize();
        }
    }
    else if (m_subBGSprite)
    {
        cocos2d::CCTexture2D* texture = cocos2d::CCTextureCache::sharedTextureCache()->addImage(filename);
        m_subBGSprite->setTexture(texture);
    }
}

void ButtonSprite::updateSpriteBGSize()
{
    if (!m_absolute)
    {
        float scaledWidth = m_subSprite->getContentSize().width * m_subSprite->getScale();
        m_width = (scaledWidth > m_minWidth) ? scaledWidth : m_minWidth;
    }
    
    cocos2d::CCSize finalSize;
    
    if (m_BGSprite)
    {
        float height = m_subSprite->getContentSize().height * m_subSprite->getScale() + 8.0f;
        if (height < 40.0f)
        {
            height = 40.0f;
        }
        
        m_BGSprite->setContentSize(cocos2d::CCSize(m_width + 8.0f, height));
        finalSize = m_BGSprite->getContentSize();
    }
    else if (m_subBGSprite)
    {
        finalSize = m_subBGSprite->getContentSize();
    }
    
    setContentSize(finalSize);
    
    cocos2d::CCPoint spritePos(
        getContentSize().width * 0.5f + m_spritePosition.x,
        getContentSize().height * 0.5f + m_spritePosition.y
    );
    m_subSprite->setPosition(spritePos);
    
    cocos2d::CCNode* bgSprite = m_BGSprite ? (cocos2d::CCNode*)m_BGSprite : (cocos2d::CCNode*)m_subBGSprite;
    if (bgSprite)
    {
        bgSprite->setPosition(cocos2d::CCPoint(getContentSize().width * 0.5f, getContentSize().height * 0.5f));
    }
    
    if (getParent())
    {
        CCMenuItemSpriteExtra* parent = dynamic_cast<CCMenuItemSpriteExtra*>(getParent());
        if (parent)
        {
            parent->setContentSize(getContentSize());
            setPosition(cocos2d::CCPoint(getContentSize().width * 0.5f, getContentSize().height * 0.5f));
        }
    }
}

void ButtonSprite::updateSpriteOffset(cocos2d::CCPoint offset)
{
    m_spritePosition = offset;
    updateSpriteBGSize();
}

ButtonSprite::~ButtonSprite()
{
}