#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include <string>

class GJBaseGameLayer;

class TextGameObject : public GameObject {
public:
    static TextGameObject* create(cocos2d::CCTexture2D* texture);
    bool init(cocos2d::CCTexture2D* texture) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    int getTextKerning();
    void updateTextKerning(int kerning);
    void updateTextObject(std::string text, bool defaultFont);
    virtual ~TextGameObject();

protected:
    int m_kerning = 0;
    std::string m_text;
};