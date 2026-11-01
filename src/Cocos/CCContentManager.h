#pragma once

#include "Platform/cocos2d.h"

class CCContentManager : public cocos2d::CCObject {
public:
    static CCContentManager* sharedManager();

    bool init();
    void clearCache();

    cocos2d::CCDictionary* addDict(const char* p0, bool p1);
    cocos2d::CCDictionary* addDictDS(const char* p0);

    ~CCContentManager();

private:
    cocos2d::CCDictionary* m_dictCache;
};