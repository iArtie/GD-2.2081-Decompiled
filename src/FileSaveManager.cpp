#include "FileSaveManager.h"

void FileSaveManager::firstLoad() {
    m_storeData = getStoreData();
    if (m_storeData) {
        m_storeData->retain();
    }
    
    cocos2d::CCArray* keys = m_storeData->allKeys();
    unsigned int keyCount = keys->count();
    
    for (unsigned int i = 0; i < keyCount; i++) {
        cocos2d::CCString* keyStr = (cocos2d::CCString*)keys->objectAtIndex(i);
        std::string key = keyStr->getCString();
        
        cocos2d::CCDictionary* dict = (cocos2d::CCDictionary*)m_storeData->objectForKey(key);
        
        // Check if this dictionary has a "category" key
        std::string categoryKey = "category";
        cocos2d::CCString* categoryStr = (cocos2d::CCString*)dict->objectForKey(categoryKey);
        
        if (categoryStr) {
            std::string category = categoryStr->getCString();
            
            // Check if category is "group"
            if (category == "group") {
                // Move all objects from this dictionary to the root
                cocos2d::CCArray* subKeys = dict->allKeys();
                for (unsigned int j = 0; j < subKeys->count(); j++) {
                    cocos2d::CCString* subKey = (cocos2d::CCString*)subKeys->objectAtIndex(j);
                    std::string subKeyStr = subKey->getCString();
                    
                    cocos2d::CCObject* obj = dict->objectForKey(subKeyStr);
                    if (obj && dynamic_cast<cocos2d::CCDictionary*>(obj)) {
                        m_storeData->setObject(obj, subKeyStr);
                    }
                }
                
                // Remove the original dictionary
                m_storeData->removeObjectForKey(key);
            }
        }
    }
}

cocos2d::CCDictionary* FileSaveManager::getStoreData() {
    return cocos2d::CCDictionary::create();
}

bool FileSaveManager::init() {
    m_fileName = "CCData.dat";
    return true;
}

void FileSaveManager::loadDataFromFile(const char* filename) {
    firstLoad();
}

FileSaveManager* FileSaveManager::sharedState() {
    static FileSaveManager* instance = nullptr;
    
    if (!instance) {
        instance = new FileSaveManager();
        if (instance && instance->init()) {
            // The instance is not autoreleased since it's a singleton
        } else {
            CC_SAFE_DELETE(instance);
        }
    }
    
    return instance;
}

FileSaveManager::~FileSaveManager() {
    if (m_storeData) {
        m_storeData->release();
        m_storeData = nullptr;
    }
}