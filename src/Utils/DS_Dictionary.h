#pragma once

#include "Platform/cocos2d.h"
#include <string>
#include <vector>
#include <map>
#include <pugixml.hpp>

class DS_Dictionary {
public:
    DS_Dictionary();
    
    void checkCompatibility();
    std::string cleanStringWhiteSpace(const std::string* p0);
    static void copyFile(const char* p0, const char* p1);
    cocos2d::CCObject* decodeObjectForKey(const char* p0, bool p1, int p2);
    std::vector<std::string>* getAllKeys(std::vector<std::string>* retstr);
    cocos2d::CCArray* getArrayForKey(const char* p0, bool p1);
    bool getBoolForKey(const char* p0);
    cocos2d::CCDictionary* getDictForKey(const char* p0, bool p1);
    float getFloatForKey(const char* p0);
    unsigned int getIndexOfKey(const char* p0);
    unsigned int getIndexOfKeyWithClosestAlphaNumericalMatch(const char* p0);
    int getIntegerForKey(const char* p0);
    std::string getKey(unsigned int p0);
    unsigned int getNumKeys();
    cocos2d::CCObject* getObjectForKey(const char* p0);
    cocos2d::CCRect getRectForKey(const char* p0);
    std::string getStringForKey(const char* p0);
    cocos2d::CCPoint getVec2ForKey(const char* p0);
    bool loadRootSubDictFromCompressedFile(const char* p0);
    bool loadRootSubDictFromFile(const char* p0);
    bool loadRootSubDictFromString(const std::string* p0);
    bool rectFromString(const std::string* p0, cocos2d::CCRect* p1);
    void removeAllKeys();
    void removeKey(const char* p0);
    bool saveRootSubDictToCompressedFile(const char* p0);
    bool saveRootSubDictToFile(const char* p0);
    std::string saveRootSubDictToString();
    void setArrayForKey(const char* p0, cocos2d::CCArray* p1);
    void setBoolForKey(const char* p0, bool p1);
    void setBoolMapForKey(const char* p0, std::map<std::string, bool>* p1);
    void setDictForKey(const char* p0, cocos2d::CCDictionary* p1);
    void setFloatForKey(const char* p0, float p1);
    void setIntegerForKey(const char* p0, int p1);
    void setObjectForKey(const char* p0, cocos2d::CCObject* p1);
    void setRectArrayForKey(const char* p0, const std::vector<cocos2d::CCRect>* p1);
    void setRectForKey(const char* p0, const cocos2d::CCRect* p1);
    void setStringArrayForKey(const char* p0, const std::vector<std::string>* p1);
    void setStringForKey(const char* p0, const std::string* p1);
    void setSubDictForKey(const char* p0);
    void setVec2ArrayForKey(const char* p0, const std::vector<cocos2d::CCPoint>* p1);
    void setVec2ForKey(const char* p0, const cocos2d::CCPoint* p1);
    void split(const std::string* p0, const char* p1, std::vector<std::string>* p2);
    bool splitWithForm(const std::string* p0, std::vector<std::string>* p1);
    void stepBackToRootSubDict();
    bool stepIntoSubDictWithKey(const char* p0);
    void stepOutOfSubDict();
    bool vec2FromString(const std::string* p0, cocos2d::CCPoint* p1);

private:
    pugi::xml_document doc;
    bool compatible;
    std::vector<pugi::xml_node> dictTree;
};