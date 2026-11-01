#pragma once

#include "cocos2d.h"
#include "SFXInfoObject.h"
#include <string>

class SFXFolderObject : public SFXInfoObject {
public:
    static SFXFolderObject* create(int id, std::string name, int folderID);
    bool init(int id, std::string name, int folderID);

    ~SFXFolderObject();

protected:
    bool m_folder = false;
    cocos2d::CCArray* m_sfxObjects = nullptr;
};