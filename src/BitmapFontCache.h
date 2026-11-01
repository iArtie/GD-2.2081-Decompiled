#pragma once

#include "CCObject.h"
#include "cocos2d.h"

class FontObject;

class BitmapFontCache : public cocos2d::CCObject {
public:
    FontObject* fontWithConfigFile(const char* font, float scale);
    bool init();
    static void purgeSharedFontCache();
    static BitmapFontCache* sharedFontCache();
    ~BitmapFontCache();

private:
    cocos2d::CCDictionary* m_cache;
};