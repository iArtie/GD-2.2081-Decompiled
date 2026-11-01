#pragma once

#include "Platform/cocos2d.h"

class ColorActionSprite : public cocos2d::CCNode {
public:
    static ColorActionSprite* create();

    bool init();

    cocos2d::ccColor3B getColor() const { return m_color; }
    cocos2d::ccColor3B getCopyColor() const { return m_copyColor; }
    float getOpacity() const { return m_opacity; }

    void setColor(const cocos2d::ccColor3B& color) { m_color = color; }
    void setCopyColor(const cocos2d::ccColor3B& color) { m_copyColor = color; }
    void setOpacity(float opacity) { m_opacity = opacity; }

    ~ColorActionSprite();

private:
    cocos2d::ccColor3B m_copyColor;
    cocos2d::ccColor3B m_color;
    float m_opacity;
};