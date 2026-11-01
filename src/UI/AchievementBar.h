#pragma once

#include "Platform/cocos2d.h"
#include "UI/TextArea.h"
#include "UI/GJItemIcon.h"

class AchievementBar : public cocos2d::CCNodeRGBA
{
public:
    static AchievementBar* create(const char* title, const char* desc, const char* icon, bool quest);
    
    bool init(const char* title, const char* desc, const char* icon, bool quest);
    void show();
    void setOpacity(GLubyte opacity) override;
    
    virtual ~AchievementBar();

private:
    cocos2d::CCLayerColor* m_layerColor;
    cocos2d::extension::CCScale9Sprite* m_bg;
    cocos2d::extension::CCScale9Sprite* m_bg2;
    cocos2d::CCSprite* m_achievementGlowSprite;
    cocos2d::CCSprite* m_achievementIconSprite;
    cocos2d::CCLabelBMFont* m_titleLabel;
    TextArea* m_achievementDescription;
    GJItemIcon* m_achievementSprite;
    cocos2d::CCScene* m_nextScene;
    int m_unkUnused;
};