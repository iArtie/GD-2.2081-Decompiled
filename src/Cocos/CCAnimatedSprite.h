#pragma once

#include "Platform/cocos2d.h"
#include <string>

class AnimatedSpriteDelegate;
class CCPartAnimSprite;
class SpriteAnimationManager;

enum spriteMode {
    FrameByFrame = 0,
    PartAnim = 1
};

class CCAnimatedSprite : public cocos2d::CCSprite {
public:
    void animationFinished(const char* key);
    void animationFinishedO(cocos2d::CCObject* str);
    void cleanupSprite();
    void runAnimation(std::string animation);
    void runAnimationForced(std::string animation);
    virtual void setColor(const cocos2d::ccColor3B* color) override;
    virtual void setOpacity(unsigned __int8 opacity) override;
    void stopTween();
    void switchToMode(spriteMode mode);
    void tweenToAnimation(std::string animation, float duration);
    void tweenToAnimationFinished();
    void willPlayAnimation();
    ~CCAnimatedSprite();

private:
    AnimatedSpriteDelegate* m_delegate;
    CCPartAnimSprite* m_paSprite;
    cocos2d::CCSprite* m_fbfSprite;
    SpriteAnimationManager* m_animationManager;
    spriteMode m_spriteMode;
    CCPartAnimSprite* m_sprite;
    std::string m_activeTween;
};