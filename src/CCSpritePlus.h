#pragma once

#include "cocos2d.h"

enum {
    GameObjectObjectType = 0
};

class CCSpritePlus : public cocos2d::CCSprite {
public:
    static CCSpritePlus* createWithSpriteFrame(cocos2d::CCSpriteFrame* frame);
    static CCSpritePlus* createWithSpriteFrameName(const char* frame);

    bool initWithSpriteFrameName(const char* frameName);
    bool initWithTexture(cocos2d::CCTexture2D* texture);

    void addFollower(cocos2d::CCNode* follower);
    void followSprite();
    CCSpritePlus* getFollower();
    void removeFollower(cocos2d::CCNode* sprite);
    void stopFollow();

    virtual void setFlipX(bool flipX) override;
    virtual void setFlipY(bool flipY) override;
    virtual void setPosition(const cocos2d::CCPoint* position) override;
    virtual void setRotation(float rotation) override;
    virtual void setRotationX(float rotationX) override;
    virtual void setRotationY(float rotationY) override;
    virtual void setScale(float scale) override;
    virtual void setScaleX(float scaleX) override;
    virtual void setScaleY(float scaleY) override;

    ~CCSpritePlus();

private:
    int m_eObjType;
    bool m_hasFollower;
    cocos2d::CCArray* m_followers;
    bool m_propagateFlipChanges;
    cocos2d::CCArray* m_pChildren;
    bool m_propagateScaleChanges;
    CCSpritePlus* m_followingSprite;
};