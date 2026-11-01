#pragma once

#include "cocos2d.h"
#include "GJEnums.h"

class GJRobotSprite;
class GJSpiderSprite;

class SimplePlayer : public cocos2d::CCSprite {
public:
    static SimplePlayer* create(int id);
    bool init(int id) override;
    void asyncLoadIcon(int id, IconType type);
    void createRobotSprite(int frame);
    void createSpiderSprite(int frame);
    void disableCustomGlowColor();
    void enableCustomGlowColor(const cocos2d::ccColor3B* color);
    void hideAll();
    void hideSecondary();
    void iconFinishedLoading(int id, IconType type);
    void setColor(const cocos2d::ccColor3B* color);
    void setColors(const cocos2d::ccColor3B* color1, const cocos2d::ccColor3B* color2);
    void setOpacity(unsigned char opacity) override;
    void setSecondColor(const cocos2d::ccColor3B* color);
    void tryEnableCustomGlowColor(int id);
    void updateColors();
    void updatePlayerFrame(int id, IconType type);
    virtual ~SimplePlayer();

protected:
    cocos2d::CCSprite* m_firstLayer = nullptr;
    cocos2d::CCSprite* m_secondLayer = nullptr;
    cocos2d::CCSprite* m_birdDome = nullptr;
    cocos2d::CCSprite* m_outlineSprite = nullptr;
    cocos2d::CCSprite* m_detailSprite = nullptr;
    GJRobotSprite* m_robotSprite = nullptr;
    GJSpiderSprite* m_spiderSprite = nullptr;
    bool m_hasCustomGlowColor = false;
    cocos2d::ccColor3B m_glowColor = {255, 255, 255};
    bool m_hasGlowOutline = false;
    int m_iconRequestID = 0;
    bool m_iconLoaded = false;
};