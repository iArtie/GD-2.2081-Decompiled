#pragma once

#include "Platform/cocos2d.h"
#include <string>

class OnlineListDelegate {
public:
    void loadListFailed(const char* key);
    void loadListFinished(cocos2d::CCArray* objects, const char* key);
    void setupPageInfo(std::string info, const char* key);
};