#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"

class UserListDelegate {
public:
    void forceReloadList(UserListType type);
    void getUserListFailed(UserListType type, GJErrorCode errorType);
    void getUserListFinished(cocos2d::CCArray* scores, UserListType type);
    void userListChanged(cocos2d::CCArray* scores, UserListType type);
};