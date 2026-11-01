// GJDailyLevelDelegate.h
#pragma once

enum class GJTimedLevelType;
enum class GJErrorCode;

class GJDailyLevelDelegate {
public:
    virtual void dailyStatusFailed(GJTimedLevelType type, GJErrorCode errorType) {}
    virtual void dailyStatusFinished(GJTimedLevelType type) {}
    virtual ~GJDailyLevelDelegate() {}
};