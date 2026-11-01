#pragma once

#include "Platform/cocos2d.h"
#include <string>

class CCAnimatedSprite;
class CCSpritePart;

class AnimatedGameObject {
public:
    void activateObject();
    void animationFinished(const char* key);
    static std::string animationForID(int type, int id);
    static AnimatedGameObject* create(int id);
    void deactivateObject(bool deactivate);
    static float getTweenTime(int type, int id);
    bool init(int id);
    void playAnimation(int type);
    void resetObject();
    void setChildColor(const cocos2d::ccColor3B* color);
    void setObjectColor(const cocos2d::ccColor3B* color);
    void setOpacity(unsigned char opacity);
    void setupAnimatedSize(int id);
    void setupChildSprites();
    void updateObjectAnimation();
    ~AnimatedGameObject();

private:
    CCSpritePart* m_eyeSpritePart;
    CCAnimatedSprite* m_animatedSprite;
    cocos2d::CCSprite* m_childSprite;
    bool m_hasNoGlow;
    bool m_isActivated;
    std::string m_currentAnimation;
    bool m_finishedAnimating;
    int m_objectID;
    int m_animationID;
    bool m_unk28c;
    bool m_colorSpriteLocked;
    cocos2d::CCSprite* m_colorSprite;
    bool m_editorEnabled;
    bool m_playingAnimation;
    bool m_hasContentSize;
    cocos2d::CCSize m_lastSize;
    float m_objectRadius;
    float m_scaleY;
    float m_width;
    float m_scaleX;
    float m_height;
    int m_classType;
    bool m_notGrounded;
    bool m_bUnkBool2;
    bool m_unk367;
};