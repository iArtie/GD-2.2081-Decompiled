#pragma once

#include "Platform/cocos2d.h"

class GJTransformControlDelegate;
class GJTransformState;
class CCMenuItemSpriteExtra;
class EditorUI;

class GJTransformControl : public cocos2d::CCLayer {
public:
    void applyRotation(float rotation);
    void calculateRotationOffset();
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    static GJTransformControl* create();
    void finishTouch();
    bool init() override;
    void loadFromState(GJTransformState* state);
    void logCurrentZeroPos();
    void onToggleLockScale(cocos2d::CCObject* sender);
    void refreshControl();
    void saveToState(GJTransformState* state);
    void scaleButtons(float scale);
    cocos2d::CCSprite* spriteByTag(int tag);
    void updateAnchorSprite(cocos2d::CCPoint position);
    void updateButtons(bool transform, bool skew);
    void updateMinMaxPositions();
    virtual ~GJTransformControl();

private:
    bool m_warpLocked;
    CCMenuItemSpriteExtra* m_warpLockButton;
    cocos2d::CCArray* m_warpSprites;
    float m_buttonScale;
    cocos2d::CCNode* m_mainNode;
    GJTransformControl* m_mainNodeParent;
    int m_touchID;
    GJTransformControlDelegate* m_delegate;
    float m_rotation;
    cocos2d::CCPoint m_rotatePosition;
    float m_rotationX;
    float m_rotationY;
    float m_scaleX;
    float m_scaleY;
    int m_transformButtonType;
    cocos2d::CCPoint m_cursorDifference;
    cocos2d::CCPoint m_topRightPosition;
    cocos2d::CCPoint m_bottomLeftPosition;
    cocos2d::CCPoint m_topRight;
    cocos2d::CCPoint m_bottomLeft;
    cocos2d::CCArray* m_objects;
    cocos2d::CCPoint m_topRightOrigin;
    cocos2d::CCPoint m_bottomLeftOrigin;
};