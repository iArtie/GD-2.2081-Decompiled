#pragma once

class CCCircleWave;

class CCCircleWaveDelegate {
public:
    virtual ~CCCircleWaveDelegate() {}

    virtual void circleWaveWillBeRemoved(CCCircleWave* circleWave);
};