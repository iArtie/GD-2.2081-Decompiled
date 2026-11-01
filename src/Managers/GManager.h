#pragma once

#include "Platform/cocos2d.h"
#include <string>

class DS_Dictionary;

class GManager : public cocos2d::CCNode {
public:
    virtual void dataLoaded(DS_Dictionary* dict);
    virtual void encodeDataTo(DS_Dictionary* dict);
    virtual void firstLoad();
    std::string getCompressedSaveString();
    std::string getSaveString();
    bool init() override;
    void load();
    void loadDataFromFile(const std::string& filename);
    void loadFromCompressedString(std::string* str);
    void loadFromString(std::string* str);
    void save();
    void saveData(DS_Dictionary* dict, const std::string& filename);
    void saveGMTo(const std::string& filename);
    void setup();
    bool tryLoadData(DS_Dictionary* dict, const std::string* filename);
    virtual ~GManager();

    bool m_setup;
    bool m_saved;
    std::string m_fileName;
};