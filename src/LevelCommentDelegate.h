#pragma once

#include <string>

class LevelCommentDelegate {
public:
    virtual ~LevelCommentDelegate() = default;
    virtual void loadCommentsFailed(const char* key);
    virtual void setupPageInfo(std::string info, const char* key);
    virtual void updateUserScoreFinished();
};