#pragma once

#include "Platform/cocos2d.h"
#include <sys/time.h>

class CCContentLayer;
class CCScrollLayerExtDelegate;

class CCScrollLayerExt : public cocos2d::CCLayer {
public:
    CCScrollLayerExt(cocos2d::CCRect rect);
    virtual ~CCScrollLayerExt();

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void registerWithTouchDispatcher();
    virtual void visit();

    void constraintContent();
    void doConstraintContent(bool instant);
    float getMaxY();
    float getMinY();
    void moveToTop();
    void moveToTopWithOffset(float offset);
    void postVisit();
    void scrollLayer(float offset);
    void scrollingEnd();
    void updateIndicators(float dt);

protected:
    CCContentLayer* m_contentLayer;
    bool m_notAtEndOfScroll;
    CCScrollLayerExtDelegate* m_delegate;
    cocos2d::CCLayerColor* m_horizontalScrollbar;
    bool m_vScrollbarVisible;
    cocos2d::CCLayerColor* m_verticalScrollbar;
    bool m_hScrollbarVisible;
    bool m_disableMovement;
    cocos2d::CCTouch* m_touch;
    cocos2d::CCPoint m_touchPosition;
    cocos2d::CCPoint m_touchStartPosition;
    timeval m_timeValue;
    bool m_touchDown;
    bool m_cutContent;
    bool m_disableVertical;
    bool m_disableHorizontal;
    float m_peekLimitTop;
    float m_peekLimitBottom;
    float m_scrollLimitBottom;
    float m_scrollLimitTop;  // Ahora es protected en lugar de private
};