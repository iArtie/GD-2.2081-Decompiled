#include "SFXFolderObject.h"

SFXFolderObject* SFXFolderObject::create(int id, std::string name, int folderID)
{
    SFXFolderObject* instance = new SFXFolderObject();
    if (instance->init(id, name, folderID))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SFXFolderObject::init(int id, std::string name, int folderID)
{
    if (!SFXInfoObject::init(id, name, folderID, 0, 0))
        return false;

    m_folder = true;
    m_sfxObjects = cocos2d::CCArray::create();
    m_sfxObjects->retain();
    return true;
}

SFXFolderObject::~SFXFolderObject()
{
}