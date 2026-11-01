#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"

class ButtonSprite : public cocos2d::CCSprite
{
public:
    static ButtonSprite* create(const char* caption);
    
    void setString(const char* text);
    void updateBGImage(const char* filename);
    void updateSpriteBGSize();
    void updateSpriteOffset(cocos2d::CCPoint offset);
    
    virtual ~ButtonSprite();

private:
    std::string m_caption;
    bool m_absolute;
    float m_width;
    float m_minWidth;
    cocos2d::CCSprite* m_subSprite;
    cocos2d::extension::CCScale9Sprite* m_BGSprite;
    cocos2d::CCSprite* m_subBGSprite;
    cocos2d::CCPoint m_spritePosition;
    int m_mode;
    cocos2d::CCLabelBMFont* m_label;
    float m_scale;
    float m_height;
    cocos2d::CCPoint m_textOffset;
};