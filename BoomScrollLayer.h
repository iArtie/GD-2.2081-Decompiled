#pragma once

#include "cocos2d.h"

class ExtendedLayer;
class BoomScrollLayerDelegate;
class DynamicScrollDelegate;

class BoomScrollLayer : public cocos2d::CCLayer {
public:
    static BoomScrollLayer* create(cocos2d::CCArray* pages, int offset, bool looped);
    static BoomScrollLayer* create(cocos2d::CCArray* pages, int offset, bool looped, bool dynamic, cocos2d::CCArray* dynamicObjects);

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void registerWithTouchDispatcher() override;
    virtual void visit() override;
    virtual ~BoomScrollLayer();

    void addPage(cocos2d::CCLayer* page);
    void addPage(cocos2d::CCLayer* page, int index);
    void cancelAndStoleTouch(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void claimTouch(cocos2d::CCTouch* touch);
    cocos2d::CCLayer* getPage(int page);
    int getRelativePageForNum(int page);
    cocos2d::CCPoint getRelativePosForPage(int page);
    int getTotalPages();
    void instantMoveToPage(int page);
    void moveToPage(int page);
    void moveToPageEnded();
    cocos2d::CCPoint positionForPageWithNumber(int page);
    void quickUpdate();
    void removePage(cocos2d::CCLayer* page);
    void removePageWithNumber(int page);
    void repositionPagesLooped();
    void selectPage(int page);
    void setDotScale(float scale);
    void setPagesIndicatorPosition(cocos2d::CCPoint pos);
    void togglePageIndicators(bool visible);
    void updateDots(float dt);
    void updatePages();

protected:
    int pageNumberForPosition(cocos2d::CCPoint pos);

private:
    bool m_doVisit;
    cocos2d::CCRect m_clippingRect;
    cocos2d::CCRect m_rect;
    cocos2d::CCTouch* m_touch;
    int m_touchType;
    float m_touchX;
    double m_touchTime;
    cocos2d::CCArray* m_dots;
    bool m_dynamic;
    cocos2d::CCArray* m_dynamicObjects;
    cocos2d::CCArray* m_pages;
    float m_pageOffset;
    ExtendedLayer* m_extendedLayer;
    cocos2d::CCPoint m_dotPosition;
    bool m_looped;
    bool m_dotsVisible;
    int m_slowPage;
    int m_page;
    BoomScrollLayerDelegate* m_delegate;
    DynamicScrollDelegate* m_dynamicDelegate;
    bool m_pagesInvisible;
    bool m_pageMoving;
    cocos2d::CCPoint m_position;
    float m_touchQuotient;
    float m_maxSpeed;
    float m_minSpeed;
    float m_unkFloat2;
    float m_minimumTouchLengthToChangePage;
    float m_unkFloat1;
    bool m_ignoreTouchCancel;
    float m_minimumTouchLengthToSlide;
    bool m_cancelAndStealTouch;
    float m_width;
};