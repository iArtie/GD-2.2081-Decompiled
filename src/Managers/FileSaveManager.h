#pragma once

#include "Platform/cocos2d.h"
#include <string>

class FileSaveManager : public cocos2d::CCNode {
public:
    void firstLoad();
    cocos2d::CCDictionary* getStoreData();
    bool init();
    void loadDataFromFile(const char* filename);
    static FileSaveManager* sharedState();
    
    virtual ~FileSaveManager();

private:
    std::string m_fileName;
    cocos2d::CCDictionary* m_storeData;
    bool m_setup;
    bool m_saved;
    bool m_quickSave;
};