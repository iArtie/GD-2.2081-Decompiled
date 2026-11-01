#pragma once

#include "cocos2d.h"
#include <string>

class SFXInfoObject : public cocos2d::CCObject {
public:
    static SFXInfoObject* create(int id, std::string name, int folderID, int size, int duration);
    bool init(int id, std::string name, int folderID, int size, int duration);

    std::string getLowerCaseName();

    ~SFXInfoObject();

    int m_sfxID = 0;
    std::string m_name;
    int m_folderID = 0;
    int m_size = 0;
    int m_duration = 0;
    bool m_folder = false;
};