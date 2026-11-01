#pragma once

#include "cocos2d.h"
#include <string>

class AudioEffectsLayer {
public:
    void audioStep(float dt);
    static AudioEffectsLayer* create(std::string audioString);
    void draw();
    cocos2d::CCSprite* getBGSquare();
    void goingDown();
    bool init(std::string audioString);
    void resetAudioVars();
    void triggerEffect(float pulse);
    void updateTweenAction(float value, const char* key);
    ~AudioEffectsLayer();

private:
    bool m_goingDown;
    float m_audioScale;
    cocos2d::CCArray* m_unk1c0;
    cocos2d::CCArray* m_unk1bc;
    float m_timeElapsed;
    cocos2d::CCSpriteBatchNode* m_batchNode;
    float m_audioPulseMod;
};