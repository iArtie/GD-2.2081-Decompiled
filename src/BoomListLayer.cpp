#include "BoomListLayer.h"
#include "BoomListView.h"
#include "cocos2d.h"

BoomListLayer* BoomListLayer::create(BoomListView* listView, const char* title)
{
    BoomListLayer* pRet = new BoomListLayer();
    if (pRet && pRet->init(listView, title))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool BoomListLayer::init(BoomListView* listView, const char* title)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0), 300.0f, 280.0f))
        return false;

    // Bottom bar
    cocos2d::CCSprite* bottomBar = cocos2d::CCSprite::createWithSpriteFrameName("blueBottomBar_001.png");
    bottomBar->setPosition(cocos2d::CCPoint(150.0f, -10.0f));
    this->addChild(bottomBar, 10);

    // Top bar
    cocos2d::CCSprite* topBar = cocos2d::CCSprite::createWithSpriteFrameName("highscore_hr_top_001.png");
    topBar->setPosition(cocos2d::CCPoint(150.0f, 280.0f));
    topBar->setAnchorPoint(cocos2d::CCPoint(0.5f, 1.0f));
    this->addChild(topBar, 10);

    // Left side bar
    cocos2d::CCSprite* leftSide = cocos2d::CCSprite::createWithSpriteFrameName("highscore_hr_side_001.png");
    leftSide->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    this->addChild(leftSide, 9);
    float sideScale = 280.0f / leftSide->getContentSize().height;
    leftSide->setScaleY(sideScale);

    // Right side bar (flipped)
    cocos2d::CCSprite* rightSide = cocos2d::CCSprite::createWithSpriteFrameName("highscore_hr_side_001.png");
    rightSide->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    rightSide->setPosition(cocos2d::CCPoint(300.0f, 0.0f));
    rightSide->setScaleY(sideScale);
    rightSide->setFlipX(true);
    this->addChild(rightSide, 9);

    // Background
    cocos2d::CCSprite* bg = cocos2d::CCSprite::createWithSpriteFrameName("fullTabBG_001.png");
    this->addChild(bg);
    bg->setPosition(cocos2d::CCPoint(topBar->getPosition().x, 293.0f));

    // Title label
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(title, "bigFont.fnt");
    this->addChild(titleLabel);
    titleLabel->setPosition(cocos2d::CCPoint(bg->getPosition().x, bg->getPosition().y - 1.0f));

    // List view
    this->addChild(listView, 6);

    return true;
}

BoomListLayer::~BoomListLayer()
{
    // Destructor
}