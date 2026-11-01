#pragma once

#include "cocos2d.h"
#include <string>

class DS_Dictionary;

class GJSmartPrefab : public cocos2d::CCObject {
public:
    bool canEncode();
    static GJSmartPrefab* create();
    static GJSmartPrefab* createWithCoder(DS_Dictionary* dict);
    void dataLoaded(DS_Dictionary* dict);
    void encodeWithCoder(DS_Dictionary* dict);
    bool init() override;
    virtual ~GJSmartPrefab();

    std::string m_prefabData;
    int m_prefabChance;
    int m_prefabID;
};