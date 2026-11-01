#include "CCContentManager.h"
#include "DS_Dictionary.h"
#include "cocos2d.h"

static CCContentManager* s_sharedManager = nullptr;

CCContentManager* CCContentManager::sharedManager() {
    if (!s_sharedManager) {
        s_sharedManager = new CCContentManager();
        if (s_sharedManager && s_sharedManager->init()) {
            s_sharedManager->autorelease();
        } else {
            delete s_sharedManager;
            s_sharedManager = nullptr;
        }
    }
    return s_sharedManager;
}

bool CCContentManager::init() {
    m_dictCache = cocos2d::CCDictionary::create();
    m_dictCache->retain();
    return true;
}

void CCContentManager::clearCache() {
    m_dictCache->removeAllObjects();
}

cocos2d::CCDictionary* CCContentManager::addDict(const char* p0, bool p1) {
    // Check if already cached
    cocos2d::CCDictionary* cached = static_cast<cocos2d::CCDictionary*>(m_dictCache->objectForKey(p0));
    if (cached) {
        return cached;
    }

    // Load dictionary from file
    DS_Dictionary* dict = new DS_Dictionary();
    dict->loadRootSubDictFromFile(p0);

    cocos2d::CCDictionary* result = dict->getDictForKey(nullptr, nullptr);

    delete dict;

    // Cache it
    m_dictCache->setObject(result, p0);

    return result;
}

cocos2d::CCDictionary* CCContentManager::addDictDS(const char* p0) {
    // Check if already cached
    cocos2d::CCDictionary* cached = static_cast<cocos2d::CCDictionary*>(m_dictCache->objectForKey(p0));
    if (cached) {
        return cached;
    }

    // Load dictionary from file
    DS_Dictionary* dict = new DS_Dictionary();
    dict->loadRootSubDictFromFile(p0);

    cocos2d::CCDictionary* result = dict->getDictForKey(nullptr, nullptr);

    delete dict;

    // Cache it
    m_dictCache->setObject(result, p0);

    return result;
}

CCContentManager::~CCContentManager() {
    CC_SAFE_RELEASE_NULL(m_dictCache);
}