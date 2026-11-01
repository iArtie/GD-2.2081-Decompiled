#pragma once

#include "cocos2d.h"
#include <string>
#include <array>

class CCSpriteWithHue : public cocos2d::CCSprite {
public:
    static CCSpriteWithHue* create(const std::string* file);
    static CCSpriteWithHue* createWithSpriteFrame(cocos2d::CCSpriteFrame* frame);
    static CCSpriteWithHue* createWithSpriteFrameName(const std::string* frameName);
    static CCSpriteWithHue* createWithTexture(cocos2d::CCTexture2D* texture);

    virtual void draw() override;
    float getAlpha();
    float getHue();
    std::string getShaderName();
    void getUniformLocations();
    void initShader();
    bool initWithSpriteFrame(cocos2d::CCSpriteFrame* frame);
    virtual bool initWithTexture(cocos2d::CCTexture2D* texture) override;
    void setCustomLuminance(float r, float g, float b);
    void setEvenLuminance(float luminance);
    void setHue(float hue);
    void setHueDegrees(float degrees);
    void setLuminance(float luminance);
    void setupDefaultSettings();
    virtual void updateColor() override;
    void updateColorMatrix();
    void updateHue(float unused);

    ~CCSpriteWithHue();

protected:
    virtual const char* shaderBody();

private:
    int m_uHueLoc;
    int m_uAlphaLoc;
    int m_uLumLoc;
    bool m_bDontDraw;
    std::array<std::array<float, 3>, 3> m_colorMatrix;
    std::array<float, 3> m_luminance;
    float m_hue;
};