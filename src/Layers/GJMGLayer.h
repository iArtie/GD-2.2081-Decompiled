#pragma once

#include "Platform/cocos2d.h"

class GJMGLayer : public cocos2d::CCLayer {
public:
    static GJMGLayer* create(int index);
    void deactivateGround();
    static float defaultYOffsetForBG2(int index);
    void draw() override;
    bool init(int index);
    void loadGroundSprites(int count, bool ground1);
    float scaleGround(float scale);
    void showGround();
    void toggleVisible01(bool visible);
    void toggleVisible02(bool visible);
    void updateGroundOpacity(unsigned char opacity, bool ground1);
    void updateGroundWidth(bool useThis);
    void updateMG01Blend(bool blend);
    void updateMG02Blend(bool blend);
    virtual ~GJMGLayer();

private:
    bool m_showGround;
    bool m_showGround1;
    bool m_showGround2;
    cocos2d::CCSprite* m_ground1Sprite;
    cocos2d::CCSprite* m_ground2Sprite;
    float m_ground2Offset;
    cocos2d::CCSpriteBatchNode* m_mg1BatchNode;
    float m_textureWidth;
    cocos2d::CCSpriteBatchNode* m_mg2BatchNode;
    bool m_cameraRotated;
    float m_groundWidth;
    float m_groundScale;
    bool m_blendMG1;
    bool m_blendMG2;
};