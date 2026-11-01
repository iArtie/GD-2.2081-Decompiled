#pragma once

#include "CCObject.h"
#include "Platform/cocos2d.h"

class DS_Dictionary;

class CCAnimateFrameCache : public cocos2d::CCObject {
public:
    static CCAnimateFrameCache* sharedSpriteFrameCache();
    void purgeSharedSpriteFrameCache();

    bool init();
    void removeSpriteFrames();

    void addCustomSpriteFramesWithFile(const char* filename);
    cocos2d::CCDictionary* addSpriteFramesWithFile(const char* filename);
    cocos2d::CCArray* spriteFrameByName(const char* name);

    ~CCAnimateFrameCache();

private:
    cocos2d::CCDictionary* m_animateFrames;
    cocos2d::CCDictionary* m_unk040;
    cocos2d::CCDictionary* m_animateDescriptions;
};