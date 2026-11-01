// GJGroundLayer.h
#pragma once

#include "cocos2d.h"

class GJGroundLayer : public cocos2d::CCLayer {
public:
    static GJGroundLayer* create(int groundID, int lineType);
    
    virtual void draw() override;
    virtual void showGround();
    virtual void fadeInGround(float duration);
    virtual void fadeOutGround(float duration);
    virtual ~GJGroundLayer();

    void createLine(int lineType);
    void deactivateGround();
    void fadeInFinished();
    float getGroundY();
    void hideShadows();
    bool init(int groundID, int lineType);
    void loadGroundSprites(int count, bool ground1);
    void positionGround(float y);
    float scaleGround(float scale);
    void toggleVisible01(bool visible);
    void toggleVisible02(bool visible);
    void updateGround01Color(cocos2d::ccColor3B color);
    void updateGround02Color(cocos2d::ccColor3B color);
    void updateGroundPos(cocos2d::CCPoint pos);
    void updateGroundWidth(bool useThis);
    void updateLineBlend(bool blend);
    void updateShadows();
    void updateShadowXPos(float leftX, float rightX);

    cocos2d::CCSprite* m_ground1Sprite;
    cocos2d::CCSprite* m_ground2Sprite;
    float m_textureWidth;
    cocos2d::CCSprite* m_lineSprite;
    bool m_showGround;
    bool m_blendLine;
    float m_ground1Offset;
    int m_lineType;
    float m_groundWidth;
    bool m_showGround1;
    bool m_showGround2;
    float m_unk1cc;
    bool m_cameraRotated;
};