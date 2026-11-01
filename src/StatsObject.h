#pragma once

#include "cocos2d.h"
#include <string>

class StatsObject : public cocos2d::CCObject {
public:
    static StatsObject* create(const char* key, int value);
    bool init(const char* key, int value);
    virtual ~StatsObject();

    std::string m_key;
    int m_value = 0;
};