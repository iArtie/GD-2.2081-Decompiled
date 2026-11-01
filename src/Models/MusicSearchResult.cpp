#include "Models/MusicSearchResult.h"
#include "Objects/OptionsObject.h"
#include "Models/SongInfoObject.h"
#include "Managers/MusicDownloadManager.h"
#include <cstring>

static int songOrderCompare(const void* a, const void* b)
{
    SongInfoObject* songA = *(SongInfoObject**)a;
    SongInfoObject* songB = *(SongInfoObject**)b;
    return songA->m_songOrder - songB->m_songOrder;
}

static int songIDCompare(const void* a, const void* b)
{
    SongInfoObject* songA = *(SongInfoObject**)a;
    SongInfoObject* songB = *(SongInfoObject**)b;
    return songA->m_songID - songB->m_songID;
}

static int songNameCompare(const void* a, const void* b)
{
    SongInfoObject* songA = *(SongInfoObject**)a;
    SongInfoObject* songB = *(SongInfoObject**)b;
    return strcmp(songA->m_songName.c_str(), songB->m_songName.c_str());
}

MusicSearchResult* MusicSearchResult::create(GJSongType songType)
{
    MusicSearchResult* result = new MusicSearchResult();
    if (result->init(songType))
    {
        result->autorelease();
        return result;
    }
    delete result;
    return nullptr;
}

bool MusicSearchResult::init(GJSongType songType)
{
    m_songType = songType;
    return true;
}

cocos2d::CCArray* MusicSearchResult::applyArtistFilters(cocos2d::CCArray* objects)
{
    if (!objects || objects->count() == 0)
        return objects;

    cocos2d::CCArray* returnVal = cocos2d::CCArray::create();
    m_artistFilter = false;

    if (!m_artistFilterObjects)
        return objects;

    unsigned int num = m_artistFilterObjects->count();
    if (num == 0)
        return objects;

    for (unsigned int i = 0; i < num; i++)
    {
        OptionsObject* option = (OptionsObject*)m_artistFilterObjects->objectAtIndex(i);
        if (!option)
            continue;

        if (option->m_enabled)
        {
            m_artistFilter = true;
            MusicDownloadManager* manager = MusicDownloadManager::sharedState();
            cocos2d::CCArray* filtered = manager->filterMusicByArtistID(option->m_optionID, objects);
            if (filtered)
            {
                for (unsigned int j = 0; j < filtered->count(); j++)
                {
                    cocos2d::CCObject* obj = filtered->objectAtIndex(j);
                    if (obj)
                        returnVal->addObject(obj);
                }
            }
        }
    }

    if (m_artistFilter)
        return returnVal;
    return objects;
}

cocos2d::CCArray* MusicSearchResult::applyTagFilters(cocos2d::CCArray* objects)
{
    m_tagFilter = false;

    if (!m_tagFilterObjects)
        return objects;

    unsigned int num = m_tagFilterObjects->count();
    if (num == 0)
        return objects;

    for (unsigned int i = 0; i < num; i++)
    {
        OptionsObject* option = (OptionsObject*)m_tagFilterObjects->objectAtIndex(i);
        if (!option)
            continue;

        if (objects->count() == 0)
            break;

        if (option->m_enabled)
        {
            m_tagFilter = true;
            MusicDownloadManager* manager = MusicDownloadManager::sharedState();
            objects = manager->filterMusicByTag(option->m_optionID, objects);
        }
    }

    return objects;
}

void MusicSearchResult::createArtistFilterObjects()
{
    if (!m_artistFilterObjects)
    {
        MusicDownloadManager* manager = MusicDownloadManager::sharedState();
        cocos2d::CCArray* allArtists = manager->getAllMusicArtists(m_optionsDelegate);

        if (m_artistFilterObjects != allArtists)
        {
            if (allArtists)
                allArtists->retain();
            if (m_artistFilterObjects)
                m_artistFilterObjects->release();
            m_artistFilterObjects = allArtists;
        }
    }
}

void MusicSearchResult::createTagFilterObjects()
{
    if (!m_tagFilterObjects)
    {
        MusicDownloadManager* manager = MusicDownloadManager::sharedState();
        cocos2d::CCArray* allTags = manager->getAllMusicTags(m_optionsDelegate);

        if (m_tagFilterObjects != allTags)
        {
            if (allTags)
                allTags->retain();
            if (m_tagFilterObjects)
                m_tagFilterObjects->release();
            m_tagFilterObjects = allTags;
        }
    }
}

void MusicSearchResult::stateChanged(OptionsObject* object)
{
    updateObjects(m_sortType);
}

void MusicSearchResult::updateObjects(AudioSortType type)
{
    m_sortType = type;
    MusicDownloadManager* manager = MusicDownloadManager::sharedState();
    cocos2d::CCArray* allMusicObjects = manager->getAllMusicObjects(m_songType);

    createTagFilterObjects();
    createArtistFilterObjects();

    cocos2d::CCArray* filesMatchingSearch;
    if (!m_searchQuery.empty())
    {
        filesMatchingSearch = getFilesMatchingSearch(allMusicObjects, m_searchQuery);
    }
    else
    {
        filesMatchingSearch = allMusicObjects;
    }

    cocos2d::CCArray* artistFiltered = applyArtistFilters(filesMatchingSearch);
    cocos2d::CCArray* tagFiltered = applyTagFilters(artistFiltered);

    updateFutureCount(tagFiltered, allMusicObjects);

    if (tagFiltered->count() > 1)
    {
        cocos2d::ccArray* data = tagFiltered->data;
        void** arr = (void**)data->arr;
        size_t num = data->num;

        if (type == AudioSortType::NameAscending)
        {
            qsort(arr, num, 8u, songNameCompare);
        }
        else if (type == AudioSortType::SongOrder)
        {
            qsort(arr, num, 8u, songOrderCompare);
        }
        else
        {
            qsort(arr, num, 8u, songIDCompare);
        }
    }

    if (tagFiltered != m_filterObjects)
    {
        if (tagFiltered)
            tagFiltered->retain();
        if (m_filterObjects)
            m_filterObjects->release();
        m_filterObjects = tagFiltered;
    }
}

void MusicSearchResult::updateFutureCount(cocos2d::CCArray* objects, cocos2d::CCArray* allObjects)
{
    m_futureCount = allObjects->count() - objects->count();
}

cocos2d::CCArray* MusicSearchResult::getFilesMatchingSearch(cocos2d::CCArray* objects, std::string query)
{
    cocos2d::CCArray* result = cocos2d::CCArray::create();
    if (!objects)
        return result;

    for (unsigned int i = 0; i < objects->count(); i++)
    {
        SongInfoObject* song = (SongInfoObject*)objects->objectAtIndex(i);
        if (song && song->m_songName.find(query) != std::string::npos)
            result->addObject(song);
    }
    return result;
}

MusicSearchResult::~MusicSearchResult()
{
}