#pragma once

#include <string>

enum GJErrorCode;

class FriendRequestDelegate {
public:
    virtual void forceReloadRequests(bool sent);
    virtual void loadFRequestsFailed(const char* key, GJErrorCode errorType);
    virtual void setupPageInfo(std::string info, const char* key);
    
    virtual ~FriendRequestDelegate() {}
};