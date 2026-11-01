#include "GJListLayer.h"
#include "BoomListView.h"

GJListLayer* GJListLayer::create(BoomListView* listView, const char* title, cocos2d::ccColor4B color, float width, float height, int style) {
    auto layer = new GJListLayer();
    if (layer && layer->init(listView, title, color, width, height, style)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool GJListLayer::init(BoomListView* listView, const char* title, cocos2d::ccColor4B color, float width, float height, int style) {
    if (!CCLayerColor::initWithColor(color, width, height)) {
        return false;
    }
    
    const char* topFrame = nullptr;
    const char* bottomFrame = nullptr;
    
    if (style == 1) {
        topFrame = "GJ_table_side_001.png";
        bottomFrame = "GJ_table_side_001.png";
    } else {
        topFrame = title ? "GJ_table_top_001.png" : "GJ_table_top02_001.png";
        bottomFrame = "GJ_table_bottom_001.png";
    }
    
    auto bottomSprite = cocos2d::CCSprite::createWithSpriteFrameName(bottomFrame);
    bottomSprite->setPosition(cocos2d::CCPoint(width * 0.5f, -10.0f));
    addChild(bottomSprite, 10);
    
    auto topSprite = cocos2d::CCSprite::createWithSpriteFrameName(topFrame);
    topSprite->setPosition(cocos2d::CCPoint(width * 0.5f, height - 5.0f + 20.0f));
    topSprite->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    addChild(topSprite, 10);
    
    if (style == 1) {
        topSprite->setScaleY((width - 2.0f) / topSprite->getTextureRect().size.height);
        topSprite->setRotation(180.0f);
        topSprite->setPosition(cocos2d::CCPoint(width * 0.5f, height - 14.0f + 20.0f));
        
        bottomSprite->setScaleY((width - 2.0f) / bottomSprite->getTextureRect().size.height);
        bottomSprite->setRotation(0.0f);
        bottomSprite->setPosition(cocos2d::CCPoint(width * 0.5f, -6.0f));
        
        cocos2d::CCPoint positions[4] = {
            cocos2d::CCPoint(-5.0f, height + 6.0f),
            cocos2d::CCPoint(width + 5.0f, height + 6.0f),
            cocos2d::CCPoint(-5.0f, -6.0f),
            cocos2d::CCPoint(width + 5.0f, -6.0f)
        };
        
        bool flipX[4] = {false, true, false, true};
        bool flipY[4] = {false, false, true, true};
        
        for (int i = 0; i < 4; ++i) {
            auto corner = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_corner_001.png");
            addChild(corner, 11);
            corner->setPosition(positions[i]);
            if (flipX[i]) {
                corner->setFlipX(true);
            }
            if (flipY[i]) {
                corner->setFlipY(true);
            }
        }
    }
    
    float sideYOffset = (style == 1) ? 1.0f : 0.0f;
    float sideHeight = (style == 1) ? height - 2.0f : height;
    
    auto leftSide = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    leftSide->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(leftSide, 9);
    leftSide->setPosition(cocos2d::CCPoint(-20.0f, sideYOffset));
    leftSide->setScaleY(sideHeight / leftSide->getTextureRect().size.height);
    
    auto rightSide = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    rightSide->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    addChild(rightSide, 9);
    rightSide->setPosition(cocos2d::CCPoint(width + 20.0f, sideYOffset));
    rightSide->setScaleY(leftSide->getScaleY());
    rightSide->setFlipX(true);
    
    if (title) {
        auto titleLabel = cocos2d::CCLabelBMFont::create(title, "bigFont.fnt");
        addChild(titleLabel, 11);
        titleLabel->limitLabelWidth(280.0f, 0.8f, 0.0f);
        cocos2d::CCPoint topPos = topSprite->getPosition();
        titleLabel->setPosition(topPos + cocos2d::CCPoint(0.0f, 3.0f));
    }
    
    if (listView) {
        m_listView = listView;
        m_listView->retain();
        addChild(m_listView, 6);
        m_listView->setTag(9);
    }
    
    return true;
}

GJListLayer::~GJListLayer() {
    if (m_listView) {
        m_listView->release();
    }
}