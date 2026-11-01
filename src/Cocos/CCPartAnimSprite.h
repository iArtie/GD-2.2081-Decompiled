#pragma once

#include "Platform/cocos2d.h"

class CCSpritePart;
class SpriteDescription;

class CCPartAnimSprite : public cocos2d::CCSprite {
public:
    void changeTextureOfID(const char* key, const char* frame);
    int countParts();
    void dirtify();
    cocos2d::CCSpriteFrame* displayFrame();
    CCSpritePart* getSpriteForKey(const char* key);
    bool isFrameDisplayed(cocos2d::CCSpriteFrame* frame);
    virtual void setBlendFunc(cocos2d::ccBlendFunc blendFunc) override;
    virtual void setDisplayFrame(cocos2d::CCSpriteFrame* frame) override;
    virtual void setFlipX(bool flipX) override;
    virtual void setFlipY(bool flipY) override;
    virtual void setOpacity(unsigned __int8 opacity) override;
    virtual void setScale(float scale) override;
    virtual void setScaleX(float scaleX) override;
    virtual void setScaleY(float scaleY) override;
    void transformSprite(SpriteDescription* description);
    void tweenSpriteTo(SpriteDescription* description, float duration);
    void tweenToFrame(cocos2d::CCSpriteFrame* frame, float duration);

    ~CCPartAnimSprite();

private:
    cocos2d::CCSpriteFrame* m_spriteFrame;
    cocos2d::ccBlendFunc m_sBlendFunc;
    cocos2d::CCDictionary* m_spritePartIDs;
    bool m_hasChanged;
    cocos2d::CCArray* m_spriteParts;
};