#include "Utils/BitmapFontCache.h"
#include "Models/FontObject.h"
#include "Platform/cocos2d.h"

static BitmapFontCache* s_sharedFontCache = nullptr;

FontObject* BitmapFontCache::fontWithConfigFile(const char* font, float scale)
{
    // Create cache key: font name + scale
    std::string key = std::string(font) + std::to_string(scale);
    
    // Check if already cached
    FontObject* cached = (FontObject*)m_cache->objectForKey(key);
    if (cached)
    {
        return cached;
    }
    
    // Create new font object
    FontObject* fontObj = FontObject::createWithConfigFile(font, scale);
    
    // Cache it
    m_cache->setObject(fontObj, key);
    
    return fontObj;
}

bool BitmapFontCache::init()
{
    m_cache = cocos2d::CCDictionary::create();
    m_cache->retain();
    return true;
}

void BitmapFontCache::purgeSharedFontCache()
{
    if (s_sharedFontCache)
    {
        s_sharedFontCache->release();
        s_sharedFontCache = nullptr;
    }
}

BitmapFontCache* BitmapFontCache::sharedFontCache()
{
    if (!s_sharedFontCache)
    {
        s_sharedFontCache = new BitmapFontCache();
        if (s_sharedFontCache && s_sharedFontCache->init())
        {
            s_sharedFontCache->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(s_sharedFontCache);
        }
    }
    return s_sharedFontCache;
}

BitmapFontCache::~BitmapFontCache()
{
    // Destructor
}