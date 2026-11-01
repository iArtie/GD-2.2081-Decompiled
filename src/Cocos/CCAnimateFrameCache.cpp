#include "Cocos/CCAnimateFrameCache.h"
#include "Utils/DS_Dictionary.h"
#include "Managers/ObjectManager.h"
#include "Platform/cocos2d.h"

static CCAnimateFrameCache* s_sharedFrameCache = nullptr;

CCAnimateFrameCache* CCAnimateFrameCache::sharedSpriteFrameCache() {
    if (!s_sharedFrameCache) {
        s_sharedFrameCache = new CCAnimateFrameCache();
        if (s_sharedFrameCache && s_sharedFrameCache->init()) {
            s_sharedFrameCache->autorelease();
        } else {
            delete s_sharedFrameCache;
            s_sharedFrameCache = nullptr;
        }
    }
    return s_sharedFrameCache;
}

void CCAnimateFrameCache::purgeSharedSpriteFrameCache() {
    if (s_sharedFrameCache) {
        s_sharedFrameCache->release();
        s_sharedFrameCache = nullptr;
    }
}

bool CCAnimateFrameCache::init() {
    m_animateFrames = cocos2d::CCDictionary::create();
    m_animateFrames->retain();

    m_unk040 = cocos2d::CCDictionary::create();
    m_unk040->retain();

    m_animateDescriptions = cocos2d::CCDictionary::create();
    m_animateDescriptions->retain();

    return true;
}

void CCAnimateFrameCache::removeSpriteFrames() {
    m_animateFrames->removeAllObjects();
    m_unk040->removeAllObjects();
    m_animateDescriptions->removeAllObjects();
}

void CCAnimateFrameCache::addCustomSpriteFramesWithFile(const char* filename) {
    // Check if already loaded
    if (m_animateDescriptions->objectForKey(filename)) {
        return;
    }

    // Mark as loaded
    m_animateDescriptions->setObject(cocos2d::CCNode::create(), filename);

    // Load the dictionary file
    DS_Dictionary* dict = new DS_Dictionary();
    dict->loadRootSubDictFromFile(filename);

    cocos2d::CCDictionary* rootDict = dict->getDictForKey(nullptr, nullptr);

    // Get copies string
    CCString* copiesStr = static_cast<CCString*>(rootDict->objectForKey("copies"));
    if (copiesStr) {
        // Parse copies string (space-separated list)
        std::string copies = copiesStr->getCString();
        std::vector<std::string> copyNames;
        std::string delimiter = " ";
        size_t pos = 0;
        std::string token;
        while ((pos = copies.find(delimiter)) != std::string::npos) {
            token = copies.substr(0, pos);
            if (!token.empty()) {
                copyNames.push_back(token);
            }
            copies.erase(0, pos + delimiter.length());
        }
        if (!copies.empty()) {
            copyNames.push_back(copies);
        }

        // Process each copy
        for (size_t i = 0; i < copyNames.size(); i++) {
            std::string searchStr = filename;
            std::string replaceStr = copyNames[i];

            // Replace "xyz" with the copy name in the dictionary
            std::string dictStr = filename;
            // Find and replace "xyz" with copy name
            size_t found = dictStr.find("xyz");
            if (found != std::string::npos) {
                dictStr.replace(found, 3, replaceStr);
            }

            CCString* key = CCString::create(replaceStr);
            cocos2d::CCDictionary* processedDict = ObjectManager::replaceAllOccurencesOfString(key, copiesStr, rootDict);

            // Add the processed dictionary
            addDict(processedDict, dictStr.c_str());
        }
    }

    delete dict;
}

cocos2d::CCDictionary* CCAnimateFrameCache::addSpriteFramesWithFile(const char* filename) {
    // Check if already loaded
    if (m_animateDescriptions->objectForKey(filename)) {
        return static_cast<cocos2d::CCDictionary*>(m_animateDescriptions->objectForKey(filename));
    }

    // Load the dictionary file
    DS_Dictionary* dict = new DS_Dictionary();
    dict->loadRootSubDictFromFile(filename);

    cocos2d::CCDictionary* result = addDict(dict, filename);
    delete dict;

    return result;
}

cocos2d::CCArray* CCAnimateFrameCache::spriteFrameByName(const char* name) {
    return static_cast<cocos2d::CCArray*>(m_animateFrames->objectForKey(name));
}

CCAnimateFrameCache::~CCAnimateFrameCache() {
    CC_SAFE_RELEASE_NULL(m_animateFrames);
    CC_SAFE_RELEASE_NULL(m_unk040);
    CC_SAFE_RELEASE_NULL(m_animateDescriptions);
}