#pragma once

#include "cocos2d.h"
#include "CCSpriteGrayscale.h"
#include "CCSpriteWithHue.h"

enum GauntletType;

class GauntletSprite : public cocos2d::CCNode {
public:
    static GauntletSprite* create(GauntletType type, bool locked);
    bool init(GauntletType type, bool locked);
    void toggleLockedSprite(bool locked);
    void addNormalSprite();
    void addLockedSprite();
    static cocos2d::ccColor3B colorForType(GauntletType type);
    static float luminanceForType(GauntletType type);
    virtual ~GauntletSprite();

private:
    GauntletType m_gauntletType;
};