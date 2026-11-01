#pragma once

#include "Platform/cocos2d.h"
#include <string>
#include <map>

struct HSV {
    float h;
    float s;
    float v;
};

struct CAState {
    cocos2d::ccColor3B m_fromColor;
    cocos2d::ccColor3B m_toColor;
    cocos2d::ccColor3B m_color;
    float m_duration;
    float m_fromOpacity;
    int m_playerColor;
    float m_toOpacity;
    int m_colorID;
    HSV m_copyHSV;
    bool m_blending;
    bool m_copyOpacity;
    int m_copyID;
    float m_deltaTime;
    bool m_paused;
    int m_uniqueID;
    float m_currentOpacity;
    bool m_legacyHSV;
};

class ColorAction : public cocos2d::CCObject {
public:
    static ColorAction* create();

    ColorAction* getCopy();
    bool isInUse();

    void loadFromState(CAState* state);
    void resetAction();
    void saveToState(CAState* state);
    void setupFromMap(std::map<std::string, std::string>* setup);
    void setupFromString(std::string setup);
    void step(float dt);

    ~ColorAction();

    // Getters
    bool isPaused() const { return m_paused; }
    bool isFinished() const { return m_stepFinished; }
    float getCurrentOpacity() const { return m_currentOpacity; }
    cocos2d::ccColor3B getCurrentColor() const { return m_color; }
    int getColorID() const { return m_colorID; }
    int getPlayerColor() const { return m_playerColor; }

    // Setters
    void setPaused(bool paused) { m_paused = paused; }
    void setColorID(int id) { m_colorID = id; }

protected:
    bool m_stepFinished;
    bool m_paused;
    float m_deltaTime;
    float m_duration;
    float m_stepInterval;
    cocos2d::ccColor3B m_fromColor;
    cocos2d::ccColor3B m_toColor;
    float m_fromOpacity;
    cocos2d::ccColor3B m_color;
    float m_toOpacity;
    float m_currentOpacity;
    int m_copyID;
    bool m_blending;
    int m_playerColor;
    int m_colorID;
    HSV m_copyHSV;
    bool m_copyOpacity;
    int m_uniqueID;
    bool m_legacyHSV;
};