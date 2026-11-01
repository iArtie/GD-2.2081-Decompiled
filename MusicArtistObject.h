#pragma once

#include "cocos2d.h"
#include <string>

class MusicArtistObject : public cocos2d::CCObject {
public:
    static MusicArtistObject* create(int id, std::string name, std::string url, std::string youtube);

    bool init(int id, std::string name, std::string url, std::string youtube);

    virtual ~MusicArtistObject();

protected:
    int m_artistID = 0;
    std::string m_artistName;
    std::string m_artistURL;
    std::string m_artistYouTube;
};