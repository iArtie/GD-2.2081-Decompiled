#pragma once

#include "cocos2d.h"

struct UIButtonConfig {
    bool m_oneButton;
    bool m_split;
    bool m_modeB;
    bool m_snap;
    bool m_player2;
    float m_scale;
    float m_radius;
    float m_deadzone;
    float m_opacity;
    int m_width;
    int m_height;
    cocos2d::CCPoint m_position;
};

class GJUINode : public cocos2d::CCNode {
public:
    int activeTouchTest(cocos2d::CCPoint position);
    static GJUINode* create(UIButtonConfig* config);
    void draw() override;
    float getButtonScale();
    uint8_t getOpacity();
    void highlightButton(int button);
    bool init(UIButtonConfig* config) override;
    void loadFromConfig(UIButtonConfig* config);
    void resetState();
    void saveToConfig(UIButtonConfig* config);
    void setOpacity(unsigned char opacity) override;
    void toggleHighlight(int button, bool highlight);
    void toggleModeB(bool modeB);
    void touchEnded();
    int touchTest(cocos2d::CCPoint position);
    void updateButtonFrames();
    void updateButtonPositions();
    void updateButtonScale(float scale);
    void updateDeadzone(int deadzone);
    void updateDragRadius(float radius);
    void updateHeight(float height);
    void updateRangePos(cocos2d::CCPoint position);
    void updateSize(float width, float height);
    void updateWidth(float width);
    virtual ~GJUINode();

private:
    bool m_drawLines;
    cocos2d::CCRect m_rect;
    bool m_modeB;
    float m_radius;
    float m_deadzone;
    cocos2d::CCSprite* m_secondSprite;
    cocos2d::CCSprite* m_firstSprite;
    bool m_oneButton;
    bool m_split;
    bool m_swiping;
    bool m_snap;
    int m_touchID;
    bool m_moving;
    int m_currentButton;
};