#pragma once

#include <string>

class GJOnlineRewardDelegate {
public:
    virtual void onlineRewardStatusFailed();
    virtual void onlineRewardStatusFinished(const std::string& key);
    virtual ~GJOnlineRewardDelegate() = default;
};