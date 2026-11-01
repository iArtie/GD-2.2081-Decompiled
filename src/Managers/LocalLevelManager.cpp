#include "Managers/LocalLevelManager.h"
#include "Utils/DS_Dictionary.h"
#include "Models/GJGameLevel.h"
#include "Models/GJLevelList.h"

static LocalLevelManager* s_sharedState = nullptr;

LocalLevelManager* LocalLevelManager::sharedState()
{
    if (!s_sharedState) {
        s_sharedState = new LocalLevelManager();
        s_sharedState->init();
    }
    return s_sharedState;
}

void LocalLevelManager::dataLoaded(DS_Dictionary* dict)
{
    cocos2d::CCArray* levelsArray = DS_Dictionary::getArrayForKey(dict, "LLM_01", false);

    if (m_localLevels) {
        unsigned int idx = 0;
        int updated = updateLevelOrder();
        int nextOrder = levelsArray->count() + 1 + updated;
        cocos2d::CCDictionary* nameGroups = getLevelsInNameGroups();

        while (idx < levelsArray->count()) {
            GJGameLevel* level = (GJGameLevel*)levelsArray->objectAtIndex(idx);
            level->m_hasBeenModified = false;

            std::string name = level->m_levelName;
            if (name.empty())
                name = "Unknown";

            cocos2d::CCArray* group = (cocos2d::CCArray*)nameGroups->objectForKey(name);

            if (group) {
                unsigned int i = 0;
                int maxRev = 0;
                while (i < group->count()) {
                    GJGameLevel* existing = (GJGameLevel*)group->objectAtIndex(i++);
                    if (maxRev < existing->m_levelRev)
                        maxRev = existing->m_levelRev;
                }

                for (unsigned int j = 0; ; ++j) {
                    if (j >= group->count()) {
                        bool nameMatch = false;
                        for (unsigned int k = 0; k < group->count(); ++k) {
                            GJGameLevel* existing = (GJGameLevel*)group->objectAtIndex(k);
                            if (existing->m_levelName == level->m_levelName) {
                                nameMatch = true;
                                break;
                            }
                        }
                        if (!nameMatch) {
                            group->addObject(level);
                            level->m_levelIndex = nextOrder--;
                        }
                        break;
                    }
                    GJGameLevel* existing = (GJGameLevel*)group->objectAtIndex(j);
                    if (existing->m_levelRev == level->m_levelRev) {
                        if (!existing->m_hasBeenModified) {
                            level->handleStatsConflict();
                            group->removeObject(existing, true);
                            group->addObject(level);
                        } else {
                            group->addObject(level);
                            existing->m_levelRev = maxRev + 1;
                            level->m_levelIndex = nextOrder--;
                        }
                        break;
                    }
                }
            } else {
                cocos2d::CCArray* newGroup = cocos2d::CCArray::create();
                newGroup->addObject(level);
                level->m_levelIndex = nextOrder;
                nameGroups->setObject(newGroup, name);
                --nextOrder;
            }
            ++idx;
        }

        m_localLevels->removeAllObjects();
        unsigned int j = 0;
        cocos2d::CCArray* keys = nameGroups->allKeys();
        while (j < keys->count()) {
            cocos2d::CCString* key = keys->stringAtIndex(j);
            std::string keyStr = key->getCString();
            cocos2d::CCArray* group = (cocos2d::CCArray*)nameGroups->objectForKey(keyStr);
            ++j;
            m_localLevels->addObjectsFromArray(group);
        }
        reorderLevels();
    } else if (levelsArray) {
        levelsArray->retain();
        if (m_localLevels)
            m_localLevels->release();
        m_localLevels = levelsArray;
    }

    cocos2d::CCArray* listsArray = DS_Dictionary::getArrayForKey(dict, "LLM_03", false);

    if (m_localLists) {
        unsigned int idx = 0;
        int updated = updateListOrder();
        int nextOrder = updated + listsArray->count() + 1;
        cocos2d::CCDictionary* listGroups = cocos2d::CCDictionary::create();

        while (idx < m_localLists->count()) {
            GJLevelList* list = (GJLevelList*)m_localLists->objectAtIndex(idx);
            std::string name = list->m_listName;
            cocos2d::CCArray* group = (cocos2d::CCArray*)listGroups->objectForKey(name);
            if (!group) {
                group = cocos2d::CCArray::create();
                listGroups->setObject(group, name);
            }
            group->addObject(list);
            ++idx;
        }

        for (unsigned int k = 0; k < listsArray->count(); ++k) {
            GJLevelList* list = (GJLevelList*)listsArray->objectAtIndex(k);
            list->m_modified = false;

            std::string name = list->m_listName;
            if (name.empty())
                name = "Unknown";

            cocos2d::CCArray* group = (cocos2d::CCArray*)listGroups->objectForKey(name);

            if (group) {
                unsigned int m = 0;
                int maxRev = 0;
                while (m < group->count()) {
                    GJLevelList* existing = (GJLevelList*)group->objectAtIndex(m++);
                    if (maxRev < existing->m_listRevision)
                        maxRev = existing->m_listRevision;
                }

                for (unsigned int n = 0; ; ++n) {
                    if (n >= group->count()) {
                        bool hasMatch = false;
                        for (unsigned int p = 0; p < group->count(); ++p) {
                            (void)group->objectAtIndex(p);
                            if (list->hasMatchingLevels()) {
                                hasMatch = true;
                                break;
                            }
                        }
                        if (!hasMatch) {
                            group->addObject(list);
                            list->m_listOrder = nextOrder--;
                        }
                        break;
                    }
                    GJLevelList* existing = (GJLevelList*)group->objectAtIndex(n);
                    if (existing->m_listRevision == list->m_listRevision) {
                        if (!existing->m_modified) {
                            list->handleStatsConflict();
                            group->removeObject(existing, true);
                            group->addObject(list);
                        } else if (!list->hasMatchingLevels()) {
                            group->addObject(list);
                            existing->m_listOrder = maxRev + 1;
                            list->m_listOrder = nextOrder--;
                        }
                        break;
                    }
                }
            } else {
                cocos2d::CCArray* newGroup = cocos2d::CCArray::create();
                newGroup->addObject(list);
                list->m_listOrder = nextOrder;
                listGroups->setObject(newGroup, name);
                --nextOrder;
            }
        }

        m_localLists->removeAllObjects();
        unsigned int j = 0;
        cocos2d::CCArray* keys = listGroups->allKeys();
        while (j < keys->count()) {
            cocos2d::CCString* key = keys->stringAtIndex(j);
            std::string keyStr = key->getCString();
            cocos2d::CCArray* group = (cocos2d::CCArray*)listGroups->objectForKey(keyStr);
            ++j;
            m_localLists->addObjectsFromArray(group);
        }
        reorderLists();
    } else if (listsArray) {
        listsArray->retain();
        if (m_localLists)
            m_localLists->release();
        m_localLists = listsArray;
    }

    if (DS_Dictionary::getIntegerForKey(dict, "LLM_02") <= 18)
        updateLevelRevision();
}

void LocalLevelManager::encodeDataTo(DS_Dictionary* dict)
{
    DS_Dictionary::setArrayForKey(dict, "LLM_01", m_localLevels);
    DS_Dictionary::setIntegerForKey(dict, "LLM_02", 48);
    DS_Dictionary::setArrayForKey(dict, "LLM_03", m_localLists);
}

void LocalLevelManager::firstLoad()
{
    cocos2d::CCArray* newLevels = cocos2d::CCArray::create();
    if (m_localLevels != newLevels) {
        if (newLevels)
            newLevels->retain();
        if (m_localLevels)
            m_localLevels->release();
        m_localLevels = newLevels;
    }
    cocos2d::CCArray* newLists = cocos2d::CCArray::create();
    if (m_localLists != newLists) {
        if (newLists)
            newLists->retain();
        if (m_localLists)
            m_localLists->release();
        m_localLists = newLists;
    }
}

cocos2d::CCDictionary* LocalLevelManager::getAllLevelsInDict()
{
    unsigned int idx = 0;
    cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
    while (idx < m_localLevels->count()) {
        cocos2d::CCObject* level = m_localLevels->objectAtIndex(idx);
        std::string key = ((GJGameLevel*)level)->m_levelName;
        if (!key.empty())
            dict->setObject(level, key);
        ++idx;
    }
    return dict;
}

cocos2d::CCArray* LocalLevelManager::getAllLevelsWithName(std::string name)
{
    unsigned int idx = 0;
    cocos2d::CCArray* ret = cocos2d::CCArray::create();
    while (idx < m_localLevels->count()) {
        GJGameLevel* level = (GJGameLevel*)m_localLevels->objectAtIndex(idx);
        if (level->m_levelName == name)
            ret->addObject(level);
        ++idx;
    }
    return ret;
}

cocos2d::CCArray* LocalLevelManager::getCreatedLevels(int folder)
{
    if (folder <= 0)
        return m_localLevels;

    cocos2d::CCArray* ret = cocos2d::CCArray::create();
    if (m_localLevels) {
        cocos2d::ccArray* data = m_localLevels->data;
        if (data->num) {
            cocos2d::CCObject** arr = data->arr;
            cocos2d::CCObject** end = &arr[data->num - 1];
            while (arr <= end) {
                cocos2d::CCObject* obj = *arr++;
                if (!obj)
                    break;
                if (static_cast<GJGameLevel*>(obj)->m_levelFolder == folder)
                    ret->addObject(obj);
            }
        }
    }
    return ret;
}

cocos2d::CCArray* LocalLevelManager::getCreatedLists(int folder)
{
    if (folder <= 0)
        return m_localLists;

    cocos2d::CCArray* ret = cocos2d::CCArray::create();
    if (m_localLists) {
        cocos2d::ccArray* data = m_localLists->data;
        if (data->num) {
            cocos2d::CCObject** arr = data->arr;
            cocos2d::CCObject** end = &arr[data->num - 1];
            while (arr <= end) {
                cocos2d::CCObject* obj = *arr++;
                if (!obj)
                    break;
                if (static_cast<GJLevelList*>(obj)->m_listFolder == folder)
                    ret->addObject(obj);
            }
        }
    }
    return ret;
}

cocos2d::CCDictionary* LocalLevelManager::getLevelsInNameGroups()
{
    unsigned int idx = 0;
    cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
    while (idx < m_localLevels->count()) {
        GJGameLevel* level = (GJGameLevel*)m_localLevels->objectAtIndex(idx);
        std::string name = level->m_levelName;
        cocos2d::CCArray* group = (cocos2d::CCArray*)dict->objectForKey(name);
        if (!group) {
            group = cocos2d::CCArray::create();
            dict->setObject(group, name);
        }
        group->addObject(level);
        ++idx;
    }
    return dict;
}

std::string LocalLevelManager::getMainLevelString(int id)
{
    tryLoadMainLevelString(id);
    auto it = m_mainLevels.find(id);
    if (it != m_mainLevels.end())
        return it->second;
    return "";
}

bool LocalLevelManager::init()
{
    m_fileName = "CCLocalLevels.dat";
    setup();
    return true;
}

void LocalLevelManager::markLevelsAsUnmodified()
{
    for (unsigned int i = 0; i < m_localLevels->count(); ++i) {
        GJGameLevel* level = (GJGameLevel*)m_localLevels->objectAtIndex(i);
        level->m_hasBeenModified = false;
    }
}

void LocalLevelManager::moveLevelToTop(GJGameLevel* level)
{
    if (level) {
        if (m_localLevels->containsObject(level)) {
            level->retain();
            m_localLevels->removeObject(level, true);
            m_localLevels->insertObject(level, 0);
            level->release();
            updateLevelOrder();
        }
    }
}

void LocalLevelManager::reorderLevels()
{
    if (m_localLevels->count())
        j_qsort(m_localLevels->data->arr, m_localLevels->data->num, 4, levelCompare);
}

void LocalLevelManager::reorderLists()
{
    if (m_localLists->count())
        j_qsort(m_localLists->data->arr, m_localLists->data->num, 4, listCompare);
}

void LocalLevelManager::setup()
{
    m_localLevels = cocos2d::CCArray::create();
    m_localLevels->retain();
    m_localLists = cocos2d::CCArray::create();
    m_localLists->retain();
}

void LocalLevelManager::tryLoadMainLevelString(int id)
{
    if (m_mainLevels.find(id) != m_mainLevels.end())
        return;

    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    cocos2d::CCString* pathStr = cocos2d::CCString::createWithFormat("levels/%i.txt", id);
    std::string path = pathStr->getCString();
    cocos2d::CCString* content = cocos2d::CCString::createWithContentsOfFile(path.c_str());
    if (content) {
        std::string data = content->getCString();
        if (!data.empty())
            m_mainLevels[id] = data;
    }
}

int LocalLevelManager::updateLevelOrder()
{
    unsigned int i;
    for (i = 0; i < m_localLevels->count(); ++i) {
        int count = m_localLevels->count();
        GJGameLevel* level = (GJGameLevel*)m_localLevels->objectAtIndex(count + ~i);
        level->m_levelIndex = i;
    }
    return i;
}

void LocalLevelManager::updateLevelRevision()
{
    unsigned int unnamedIdx = 0;

    for (unsigned int i = 0; i < m_localLevels->count(); ++i) {
        GJGameLevel* level = (GJGameLevel*)m_localLevels->objectAtIndex(i);
        if (level->m_levelName.empty()) {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("Unnamed %i", unnamedIdx);
            level->m_levelName = str->getCString();
            ++unnamedIdx;
        }
    }

    unsigned int i = 0;
    cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
    while (i < m_localLevels->count()) {
        int rev = 0;
        GJGameLevel* level = (GJGameLevel*)m_localLevels->objectAtIndex(i);
        while (true) {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("rev_%i_%s", rev, level->m_levelName.c_str());
            std::string key = str->getCString();
            if (dict->objectForKey(key))
                break;
            cocos2d::CCNode* node = cocos2d::CCNode::create();
            dict->setObject(node, key);
            ++rev;
        }
        level->m_levelRev = rev;
        ++i;
    }
}

int LocalLevelManager::updateListOrder()
{
    unsigned int i;
    for (i = 0; i < m_localLists->count(); ++i) {
        int count = m_localLists->count();
        GJLevelList* list = (GJLevelList*)m_localLists->objectAtIndex(count + ~i);
        list->m_listOrder = i;
    }
    return i;
}

LocalLevelManager::~LocalLevelManager()
{
    if (m_localLevels)
        m_localLevels->release();
    if (m_localLists)
        m_localLists->release();
}