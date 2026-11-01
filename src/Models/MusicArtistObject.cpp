#include "Models/MusicArtistObject.h"

MusicArtistObject* MusicArtistObject::create(int id, std::string name, std::string url, std::string youtube)
{
    MusicArtistObject* ret = new MusicArtistObject();
    if (ret->init(id, name, url, youtube)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool MusicArtistObject::init(int id, std::string name, std::string url, std::string youtube)
{
    m_artistID = id;
    m_artistName = name;
    m_artistURL = url;
    m_artistYouTube = youtube;
    return true;
}

MusicArtistObject::~MusicArtistObject()
{
}