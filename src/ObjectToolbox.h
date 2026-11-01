#pragma once

#include "cocos2d.h"
#include <string>
#include <map>

class ObjectToolbox : public cocos2d::CCNode {
public:
    static ObjectToolbox* sharedState();

    bool init() override;

    cocos2d::CCArray* allKeys();
    float gridNodeSizeForKey(int key);
    const char* intKeyToFrame(int key);
    const char* perspectiveBlockFrame(int key);

    virtual ~ObjectToolbox();

protected:
    std::map<int, std::string> m_allKeys;
};