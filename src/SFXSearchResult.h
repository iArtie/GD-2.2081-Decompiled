#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "MusicSearchResult.h"

class SFXFolderObject;
class SFXInfoObject;

class SFXSearchResult : public MusicSearchResult {
public:
    static SFXSearchResult* create(int folderID);
    bool init(int folderID);

    SFXFolderObject* getActiveFolder();
    int getSelectedPage(int id, int count);
    void setActiveFolder(SFXFolderObject* folder);
    void updateObjects(AudioSortType type) override;

    ~SFXSearchResult();

    int m_folderID = 0;
    int m_audioType = 0;
    SFXFolderObject* m_folderObject = nullptr;
    cocos2d::CCArray* m_filterObjects = nullptr;
    std::string m_searchQuery;
    int m_sortType = -1;
    int m_startIndex = 0;
};