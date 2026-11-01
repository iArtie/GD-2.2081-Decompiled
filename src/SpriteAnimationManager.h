#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include <string>

class CCAnimatedSprite;

class SpriteAnimationManager : public cocos2d::CCNode {
public:
    static SpriteAnimationManager* createWithOwner(CCAnimatedSprite* sprite, std::string definition);
    static void createAnimations(std::string definition);
    void animationFinished();
    void callAnimationFinished();
    void doCleanup();
    void executeAnimation(std::string animation);
    void finishAnimation(std::string animation);
    spriteMode getAnimType(std::string animation);
    int getPrio(std::string animation);
    void loadAnimations(std::string definition);
    void offsetCurrentAnimation(float dt);
    void overridePrio();
    void playSound(std::string sound);
    void playSoundForAnimation(std::string animation);
    void queueAnimation(std::string animation);
    void resetAnimState();
    void runAnimation(std::string animation);
    void runQueuedAnimation();
    void stopAnimations();
    void switchToFirstFrameOfAnimation(std::string animation);
    void updateAnimationSpeed(float speed);
    virtual ~SpriteAnimationManager();

protected:
    CCAnimatedSprite* m_sprite = nullptr;
    std::string m_currentAnimation;
    std::string m_nextAnimation;
    float m_speed = 1.0f;
    cocos2d::CCDictionary* m_animateDict = nullptr;
    cocos2d::CCDictionary* m_soundDict = nullptr;
    cocos2d::CCDictionary* m_priorityDict = nullptr;
    std::string m_queuedAnimation;
    cocos2d::CCDictionary* m_typeDict = nullptr;
    cocos2d::CCDictionary* m_frameDict = nullptr;
};