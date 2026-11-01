#pragma once

class GJRewardDelegate {
public:
    virtual void rewardsStatusFailed();
    virtual void rewardsStatusFinished(int type);
    virtual ~GJRewardDelegate() = default;
};