#pragma once

#include <string>

class GJBaseGameLayer;

enum AudioLineSpeed {
    Normal = 0,
    Slow = 1,
    Fast = 2
};

class AudioLineGuideGameObject {
public:
    static AudioLineGuideGameObject* create();
    std::string getSaveString(GJBaseGameLayer* layer);
    bool init();
    ~AudioLineGuideGameObject();

private:
    double m_duration;
    int m_beatsPerMinute;
    int m_beatsPerBar;
    AudioLineSpeed m_speed;
    bool m_disabled;
};