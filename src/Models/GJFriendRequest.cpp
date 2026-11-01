// GJFriendRequest.cpp
#include "Models/GJFriendRequest.h"
#include "Utils/LevelTools.h"

GJFriendRequest* GJFriendRequest::create() {
    // Note: The decompiled code expects a CCDictionary from somewhere
    // (likely a global or member variable). This is a simplified version.
    GJFriendRequest* instance = new GJFriendRequest();
    if (instance) {
        instance->autorelease();
        // In the actual decompiled code, this parses a CCDictionary
        // with keys "32", "33", "34", "35", "36", "37"
    }
    return instance;
}

GJFriendRequest::~GJFriendRequest() {
    // Base class handles cleanup
}