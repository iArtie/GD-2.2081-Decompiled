#include "FriendRequestDelegate.h"

void FriendRequestDelegate::forceReloadRequests(bool sent) {
    // Empty implementation - to be overridden by subclasses
}

void FriendRequestDelegate::loadFRequestsFailed(const char* key, GJErrorCode errorType) {
    // Empty implementation - to be overridden by subclasses
}

void FriendRequestDelegate::setupPageInfo(std::string info, const char* key) {
    // Empty implementation - to be overridden by subclasses
}