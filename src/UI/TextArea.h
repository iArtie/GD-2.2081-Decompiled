#pragma once

#include "Platform/cocos2d.h"
#include <string>

class TextAreaDelegate;
class MultilineBitmapFont;

class TextArea : public cocos2d::CCSprite
{
public:
    static TextArea* create(const std::string& text, const char* fontFile, float scale, float width, const cocos2d::CCPoint& anchorPoint, float height, int unknown);
    
    void draw();
    float fadeIn(float duration, bool unused);
    void fadeOut(float duration);
    void fadeOutAndRemove();
    void finishFade();
    void hideAll();
    void setIgnoreColorCode(bool ignore);
    void setOpacity(unsigned char opacity);
    void setString(const std::string& text);
    void showAll();
    void stopAllCharacterActions();
    void update(float dt);
    
    virtual ~TextArea();

private:
    TextAreaDelegate* m_delegate;
    bool m_allShown;
    float m_shakeElapsed;
    cocos2d::CCDictionary* m_shakeCharacters;
    MultilineBitmapFont* m_label;
    bool m_disableColor;
    std::string m_fontFile;
    float m_scale;
    float m_width;
    cocos2d::CCPoint m_anchorPoint;
    float m_height;
    float m_rectWidth;
    float m_rectHeight;
    cocos2d::CCPoint m_unkPoint;
    float m_maxWidth;
};