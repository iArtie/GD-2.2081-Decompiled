#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include <string>

class MessageListDelegate {
public:
    void forceReloadMessages(bool sent);
    void loadMessagesFailed(const char* key, GJErrorCode errorType);
    void loadMessagesFinished(cocos2d::CCArray* messages, const char* key);
    void setupPageInfo(std::string info, const char* key);
};