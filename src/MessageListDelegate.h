#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include <string>

class MessageListDelegate {
public:
    void forceReloadMessages(bool sent);
    void loadMessagesFailed(const char* key, GJErrorCode errorType);
    void loadMessagesFinished(cocos2d::CCArray* messages, const char* key);
    void setupPageInfo(std::string info, const char* key);
};