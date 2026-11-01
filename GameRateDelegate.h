#pragma once

class GameRateDelegate {
public:
    virtual ~GameRateDelegate() {}
    virtual void updateRate();
};