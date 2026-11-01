#include "BonusDropdown.h"
#include "PriceLabel.h"
#include "cocos2d.h"

BonusDropdown* BonusDropdown::create(std::string text, int price)
{
    BonusDropdown* pRet = new BonusDropdown();
    if (pRet && pRet->init(text, price))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool BonusDropdown::init(std::string text, int price)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    // Background sprite
    cocos2d::CCSprite* bg = cocos2d::CCSprite::createWithSpriteFrameName("shopTabBG_001.png");
    bg->setPosition(cocos2d::CCPoint(
        bg->getContentSize().width * 0.5f,
        bg->getContentSize().height * 0.5f
    ));
    bg->setScaleX(1.8f);

    float bgHeight = bg->getContentSize().height;
    m_yPosition = bgHeight - 2.0f;

    // Layer color background
    cocos2d::CCLayerColor* layerColor = cocos2d::CCLayerColor::create(
        cocos2d::ccc4(0, 0, 0, 0),
        winSize.width,
        bgHeight - 2.0f
    );
    m_layerColor = layerColor;
    this->addChild(layerColor);

    layerColor->setPosition(cocos2d::CCPoint(0.0f, winSize.height));
    layerColor->addChild(bg);

    // Price label (right side)
    PriceLabel* priceLabel = PriceLabel::create(price);
    cocos2d::CCPoint bgPos = bg->getPosition();
    priceLabel->setPosition(cocos2d::CCPoint(bgPos.x + 100.0f, bgPos.y - 4.0f));
    priceLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    priceLabel->setScale(0.9f);
    layerColor->addChild(priceLabel);

    // Text label (left side)
    cocos2d::CCLabelBMFont* textLabel = cocos2d::CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    textLabel->setPosition(cocos2d::CCPoint(bgPos.x - 134.0f, bgPos.y - 1.0f));
    textLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    textLabel->setScale(0.9f);
    layerColor->addChild(textLabel, 2);

    return true;
}

void BonusDropdown::show()
{
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    
    if (!m_scene)
    {
        m_scene = runningScene;
    }
    
    m_scene->addChild(this, 105);

    // Animate: slide down, wait, slide up
    cocos2d::CCMoveBy* moveDown = cocos2d::CCMoveBy::create(1.0f, cocos2d::CCPoint(0.0f, -m_yPosition));
    cocos2d::CCEaseInOut* easeDown = cocos2d::CCEaseInOut::create(moveDown, 2.0f);
    
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(1.5f);
    
    cocos2d::CCMoveBy* moveUp = cocos2d::CCMoveBy::create(1.0f, cocos2d::CCPoint(0.0f, m_yPosition));
    cocos2d::CCEaseInOut* easeUp = cocos2d::CCEaseInOut::create(moveUp, 2.0f);
    
    cocos2d::CCCallFunc* removeFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(BonusDropdown::removeFromParent));
    
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
        easeDown,
        delay,
        easeUp,
        removeFunc,
        nullptr
    );
    
    m_layerColor->runAction(sequence);
}

BonusDropdown::~BonusDropdown()
{
    // Destructor - calls parent CCNode destructor
}