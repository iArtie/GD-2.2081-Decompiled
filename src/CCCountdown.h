#pragma once

#include "cocos2d.h"

class CCCountdown : public cocos2d::CCSprite {
public:
    static CCCountdown* create();

    bool init();
    void lapFinished();
    virtual void setOpacity(unsigned __int8 opacity) override;

    void startCountdown(cocos2d::CCNode* target, cocos2d::SEL_CallFunc callback, int seconds);

    ~CCCountdown();

private:
    cocos2d::CCProgressTimer* m_progressTimer;
    cocos2d::CCLabelBMFont* m_countdownLabel;
    int m_seconds;
    cocos2d::CCNode* m_targetNode;
    cocos2d::SEL_CallFunc m_callbackSelector;
};