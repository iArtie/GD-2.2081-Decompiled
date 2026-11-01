#include "PriceLabel.h"

PriceLabel* PriceLabel::create(int price)
{
    PriceLabel* returnVal = new PriceLabel();

    if (returnVal->init(price))
    {
        returnVal->autorelease();
    }
    else
    {
        delete returnVal;
        returnVal = nullptr;
    }

    return returnVal;
}

bool PriceLabel::init(int price)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", price);

    m_priceLabel = cocos2d::CCLabelBMFont::create(str->getCString(), "bigFont.fnt");
    addChild(m_priceLabel);

    m_coinSprite = cocos2d::CCSprite::createWithSpriteFrameName("spinItem_coin01_001.png");
    addChild(m_coinSprite, 20);
    m_coinSprite->setScale(0.4f);

    cocos2d::CCSize labelSize = m_priceLabel->getContentSize();
    cocos2d::CCSize contentSize(0.0f, labelSize.height);
    setContentSize(contentSize);

    setPrice(price);
    return true;
}

void PriceLabel::setOpacity(float opacity)
{
    unsigned __int8 opacityByte = (unsigned __int8)(unsigned int)opacity;
    m_priceLabel->setOpacity(opacityByte);
    m_coinSprite->setOpacity(opacityByte);
}

void PriceLabel::setPrice(int price)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", price);
    m_priceLabel->setString(str->getCString());

    float labelWidth = m_priceLabel->getContentSize().width;
    float coinWidth = m_coinSprite->m_obRect.size.width * m_coinSprite->getScale();
    float totalWidth = labelWidth + coinWidth;

    float labelX = -(labelWidth * 0.5f - totalWidth * 0.5f);
    cocos2d::CCSize mySize = getContentSize();
    m_priceLabel->setPosition(cocos2d::CCPoint(labelX, mySize.height * 0.5f));

    float coinX = -(coinWidth * 0.5f - totalWidth * 0.5f) + 1.0f;
    cocos2d::CCSize mySize2 = getContentSize();
    m_coinSprite->setPosition(cocos2d::CCPoint(coinX, mySize2.height * 0.5f + 5.0f));
}

PriceLabel::~PriceLabel()
{
}