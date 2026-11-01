#pragma once

class LeaderboardManagerDelegate {
public:
    virtual void loadLeaderboardFailed(const char* key);
    virtual void updateUserScoreFailed();
    virtual void updateUserScoreFinished();
    virtual ~LeaderboardManagerDelegate() = default;
};