#include "SFXSearchResult.h"
#include "SFXFolderObject.h"
#include "SFXInfoObject.h"
#include "MusicDownloadManager.h"
#include "GameManager.h"

SFXSearchResult* SFXSearchResult::create(int folderID)
{
    SFXSearchResult* instance = new SFXSearchResult();
    if (instance->init(folderID))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SFXFolderObject* SFXSearchResult::getActiveFolder()
{
    return m_folderObject;
}

int SFXSearchResult::getSelectedPage(int id, int count)
{
    if (!m_filterObjects || count <= 0)
        return 0;

    for (unsigned int i = 0; i < m_filterObjects->count(); i++)
    {
        SFXInfoObject* obj = (SFXInfoObject*)m_filterObjects->objectAtIndex(i);
        if (obj->m_sfxID == id)
            return (int)floorf((float)(i / count));
    }

    return 0;
}

bool SFXSearchResult::init(int folderID)
{
    if (!MusicSearchResult::init(MusicType::SFX))
        return false;

    m_folderID = folderID;
    m_audioType = 1;
    return true;
}

void SFXSearchResult::setActiveFolder(SFXFolderObject* folder)
{
    if (folder)
    {
        if (m_folderObject != folder)
        {
            if (m_folderObject)
                m_folderObject->release();

            m_folderObject = folder;
            m_folderObject->retain();
        }
    }
    else
    {
        if (m_folderObject)
            m_folderObject->release();

        m_folderObject = nullptr;
    }
}

void SFXSearchResult::updateObjects(AudioSortType type)
{
    m_sortType = type;

    cocos2d::CCArray* sourceObjects = nullptr;

    if (m_folderObject)
    {
        if (m_searchQuery.empty())
        {
            sourceObjects = m_folderObject->m_sfxObjects;
        }
        else
        {
            int folderID = m_folderObject->m_sfxID;
            if (folderID != 1)
            {
                cocos2d::CCArray* files = cocos2d::CCArray::create();
                cocos2d::CCArray* foldersToProcess = cocos2d::CCArray::createWithObject(m_folderObject);

                while (foldersToProcess->count() > 0)
                {
                    SFXFolderObject* folder = (SFXFolderObject*)foldersToProcess->lastObject();
                    foldersToProcess->removeLastObject();

                    if (folder->m_sfxObjects)
                    {
                        for (unsigned int i = 0; i < folder->m_sfxObjects->count(); i++)
                        {
                            SFXInfoObject* info = (SFXInfoObject*)folder->m_sfxObjects->objectAtIndex(i);
                            if (info->m_folder)
                                foldersToProcess->addObject(info);
                            else
                                files->addObject(info);
                        }
                    }
                }

                sourceObjects = files;
            }
        }
    }

    if (!sourceObjects)
    {
        sourceObjects = MusicDownloadManager::sharedState()->getAllSFXObjects(true);
    }

    cocos2d::CCArray* filteredObjects = cocos2d::CCArray::createWithCapacity(sourceObjects->count());

    if (sourceObjects)
    {
        for (unsigned int i = 0; i < sourceObjects->count(); i++)
        {
            filteredObjects->addObject(sourceObjects->objectAtIndex(i));
        }
    }

    if (m_searchQuery.empty())
    {
        if (filteredObjects->count() > 1)
        {
            int (*compareFunc)(const void*, const void*) = nullptr;

            switch (type)
            {
                case AudioSortType::NameDescending: compareFunc = (int(*)(const void*, const void*))sfxNameCompareDesc; break;
                case AudioSortType::LengthAscending: compareFunc = (int(*)(const void*, const void*))sfxLengthCompare; break;
                case AudioSortType::LengthDescending: compareFunc = (int(*)(const void*, const void*))sfxLengthCompareDesc; break;
                case AudioSortType::IDAscending: compareFunc = (int(*)(const void*, const void*))sfxIDCompare; break;
                case AudioSortType::IDDescending: compareFunc = (int(*)(const void*, const void*))sfxIDCompareDesc; break;
                default: compareFunc = (int(*)(const void*, const void*))sfxNameCompare; break;
            }

            qsort(filteredObjects->data->arr, filteredObjects->data->num, 8, compareFunc);
        }

        if (filteredObjects != m_filterObjects)
        {
            if (filteredObjects)
                filteredObjects->retain();

            if (m_filterObjects)
                m_filterObjects->release();

            m_filterObjects = filteredObjects;
        }

        return;
    }

    for (unsigned int i = 0; i < filteredObjects->count(); i++)
    {
        SFXInfoObject* info = (SFXInfoObject*)filteredObjects->objectAtIndex(i);
        info->m_unk049 = false;
    }

    std::string lowerQuery = m_searchQuery;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
        [](unsigned char c) { return (char)std::tolower(c); });

    if (GameManager::sharedState()->getGameVariable("0147") && m_folderObject)
    {
        cocos2d::CCArray* foldersToProcess = cocos2d::CCArray::createWithObject(m_folderObject);

        while (foldersToProcess->count() > 0)
        {
            SFXFolderObject* folder = (SFXFolderObject*)foldersToProcess->lastObject();
            foldersToProcess->removeLastObject();

            std::string lowerName = folder->getLowerCaseName();
            bool matches = folder->m_unk049 || (lowerName.find(lowerQuery) != std::string::npos);

            if (folder->m_sfxObjects)
            {
                for (unsigned int i = 0; i < folder->m_sfxObjects->count(); i++)
                {
                    SFXInfoObject* info = (SFXInfoObject*)folder->m_sfxObjects->objectAtIndex(i);
                    info->m_unk049 = matches;
                    if (info->m_folder)
                        foldersToProcess->addObject(info);
                }
            }
        }
    }

    cocos2d::CCArray* matchingFiles = MusicSearchResult::getFilesMatchingSearch(filteredObjects, m_searchQuery);

    if (matchingFiles)
    {
        for (unsigned int i = 0; i < matchingFiles->count(); i++)
        {
            SFXInfoObject* info = (SFXInfoObject*)matchingFiles->objectAtIndex(i);
            info->m_unk049 = false;
        }
    }

    if (filteredObjects->count() > 1)
    {
        int (*compareFunc)(const void*, const void*) = nullptr;

        switch (type)
        {
            case AudioSortType::NameDescending: compareFunc = (int(*)(const void*, const void*))sfxNameCompareDesc; break;
            case AudioSortType::LengthAscending: compareFunc = (int(*)(const void*, const void*))sfxLengthCompare; break;
            case AudioSortType::LengthDescending: compareFunc = (int(*)(const void*, const void*))sfxLengthCompareDesc; break;
            case AudioSortType::IDAscending: compareFunc = (int(*)(const void*, const void*))sfxIDCompare; break;
            case AudioSortType::IDDescending: compareFunc = (int(*)(const void*, const void*))sfxIDCompareDesc; break;
            default: compareFunc = (int(*)(const void*, const void*))sfxNameCompare; break;
        }

        qsort(filteredObjects->data->arr, filteredObjects->data->num, 8, compareFunc);
    }

    if (filteredObjects != m_filterObjects)
    {
        if (filteredObjects)
            filteredObjects->retain();

        if (m_filterObjects)
            m_filterObjects->release();

        m_filterObjects = filteredObjects;
    }
}

SFXSearchResult::~SFXSearchResult()
{
}