#pragma once

#include "Platform/cocos2d.h"

class ObjectManager : public cocos2d::CCNode {
public:
    static ObjectManager* instance();

    bool init() override;
    void setup();

    bool animLoaded(const char* anim);
    cocos2d::CCDictionary* getDefinition(const char* definitionKey);
    cocos2d::CCDictionary* getGlobalAnimCopy(const char* anim);
    void loadCopiedAnimations();
    void loadCopiedSets();
    static void purgeObjectManager();
    void setLoaded(const char* objectName);

    virtual ~ObjectManager();

protected:
    cocos2d::CCDictionary* m_objectDefinitions = nullptr;
    cocos2d::CCDictionary* m_loadedAnimations = nullptr;
};