// GJFriendRequest.h
#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJFriendRequest : public cocos2d::CCObject {
public:
    static GJFriendRequest* create();
    virtual ~GJFriendRequest();

    int m_requestID;          // "32"
    int m_accountID;          // "33"
    int m_unusedToAccountID;  // "34"
    std::string m_message;    // "35" (base64 decode)
    std::string m_uploadDate; // "37"
    bool m_isRead;            // "36"
};