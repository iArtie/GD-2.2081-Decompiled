#pragma once

#include "Platform/cocos2d.h"

enum FormatterType {
    FormatterType_Int = 0,
    FormatterType_Float = 1
};

class CCCounterLabel : public cocos2d::CCLabelBMFont {
public:
    static CCCounterLabel* create(int count, const char* font, FormatterType type);

    bool init(int count, const char* font, FormatterType type);
    void calculateStepSize(int steps);
    void disableCounter();
    void enableCounter();
    void fastUpdateCounter();
    int getTargetCount();
    void setTargetCount(int count);
    void updateCounter(float dt);
    void updateString();

    ~CCCounterLabel();

private:
    bool m_counterEnabled;
    bool m_dontSchedule;
    int m_targetCount;
    int m_currentCount;
    float m_stepSize;
    int m_stepCount;
    FormatterType m_formatterType;
};