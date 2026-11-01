#pragma once

#include "Platform/cocos2d.h"

class ScrollingLayer : public cocos2d::CCLayerColor {
public:
    static ScrollingLayer* create(cocos2d::CCSize size, cocos2d::CCPoint position, float factor);

    bool init(cocos2d::CCSize size, cocos2d::CCPoint position, float factor) override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    void draw() override;
    void visit() override;

    cocos2d::CCRect getViewRect();
    void setStartOffset(cocos2d::CCPoint offset);

    virtual ~ScrollingLayer();

protected:
    int m_touchID = -1;
    cocos2d::CCPoint m_position;
    cocos2d::CCSize m_size;
    cocos2d::CCPoint m_touchPosition;
    cocos2d::CCLayer* m_contentLayer = nullptr;
    cocos2d::CCPoint m_startOffset;
    float m_scrollFactor = 0.0f;
    cocos2d::CCLayer* m_parentLayer = nullptr;
    cocos2d::CCPoint m_touchStartPosition;
};