#pragma once

#include "Platform/cocos2d.h"

class CCPartAnimSprite;
class CCSpritePart;
class GJRobotSprite : public cocos2d::CCSprite {
public:
    static GJRobotSprite* create(int frame);
    void hideGlow();
    void hideSecondary();
    bool init(int frame);
    void setOpacity(unsigned char opacity) override;
    void showGlow();
    void updateColor01(cocos2d::ccColor3B color);
    void updateColor02(cocos2d::ccColor3B color);
    void updateColors();
    void updateFrame(int frame);
    virtual ~GJRobotSprite();

private:
    cocos2d::CCArray* m_secondArray;
    cocos2d::CCSprite* m_extraSprite;
    CCPartAnimSprite* m_paSprite;
    cocos2d::CCSprite* m_glowSprite;
    IconType m_iconType;
    cocos2d::ccColor3B m_color;
    cocos2d::ccColor3B m_secondColor;
    int m_iconRequestID;
    CCSpritePart* m_headSprite;
    bool m_hasExtra;
};