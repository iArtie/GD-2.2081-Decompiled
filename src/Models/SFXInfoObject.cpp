#include "Models/SFXInfoObject.h"
#include <algorithm>
#include <cctype>

SFXInfoObject* SFXInfoObject::create(int id, std::string name, int folderID, int size, int duration)
{
    SFXInfoObject* instance = new SFXInfoObject();
    if (instance->init(id, name, folderID, size, duration))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

std::string SFXInfoObject::getLowerCaseName()
{
    std::string lowerName = m_name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
        [](unsigned char c) { return (char)std::tolower(c); });
    return lowerName;
}

bool SFXInfoObject::init(int id, std::string name, int folderID, int size, int duration)
{
    m_sfxID = id;
    m_name = name;
    m_folderID = folderID;
    m_size = size;
    m_duration = duration;
    m_folder = false;
    return true;
}

SFXInfoObject::~SFXInfoObject()
{
}