#include "GJLevelList.h"
#include "GJGameLevel.h"
#include "GameStatsManager.h"
#include "GameToolbox.h"
#include "LevelTools.h"
#include "GameLevelManager.h"
#include "FLAlertLayer.h"

int GJLevelList::s_nextId = 0;

void GJLevelList::addLevelToList(GJGameLevel* level) {
    int levelId = level->m_levelID.random - level->m_levelID.seed;
    
    for (int id : m_levels) {
        if (id == levelId) {
            return;
        }
    }
    
    m_levels.push_back(levelId);
    updateLevelsString();
    m_levelsDict->setObject(GJGameLevel::getListSnapshot(level), GameToolbox::intToString(levelId));
    m_modified = true;
}

bool GJLevelList::canEncode() {
    return true;
}

int GJLevelList::completedLevels() {
    int count = 0;
    auto stats = GameStatsManager::sharedState();
    for (int id : m_levels) {
        if (stats->hasCompletedOnlineLevel(id)) {
            ++count;
        }
    }
    return count;
}

GJLevelList* GJLevelList::create() {
    auto list = new GJLevelList();
    if (list && list->init()) {
        list->autorelease();
        return list;
    }
    delete list;
    return nullptr;
}

GJLevelList* GJLevelList::createWithCoder(DS_Dictionary* dict) {
    auto list = GJLevelList::create();
    list->dataLoaded(dict);
    return list;
}

void GJLevelList::dataLoaded(DS_Dictionary* dict) {
    m_listID = dict->getIntegerForKey("k1");
    m_folder = dict->getIntegerForKey("k84");
    m_listName = dict->getStringForKey("k2");
    m_listDesc = dict->getStringForKey("k3");
    m_creatorName = dict->getStringForKey("k5");
    m_accountID = dict->getIntegerForKey("k60");
    m_difficulty = dict->getIntegerForKey("k7");
    m_downloads = dict->getIntegerForKey("k11");
    m_uploaded = dict->getBoolForKey("k15");
    m_likes = dict->getIntegerForKey("k22");
    m_listType = dict->getIntegerForKey("k21");
    m_listVersion = dict->getIntegerForKey("k16");
    m_unlisted = dict->getBoolForKey("k79");
    m_friendsOnly = dict->getBoolForKey("k94");
    m_listRevision = dict->getIntegerForKey("k46");
    m_modified = dict->getBoolForKey("k47");
    m_listOrder = dict->getIntegerForKey("k83");
    m_favorite = dict->getBoolForKey("k82");
    m_featured = dict->getBoolForKey("k27");
    m_uploadDate = dict->getIntegerForKey("k98");
    m_updateDate = dict->getIntegerForKey("k99");
    m_original = dict->getIntegerForKey("k42");
    m_onlineLevelsLoaded = dict->getBoolForKey("k100");
    m_diamonds = dict->getIntegerForKey("k113");
    m_levelsToClaim = dict->getIntegerForKey("k114");
    
    auto dictForKey = dict->getDictForKey("k97");
    if (dictForKey != m_levelsDict) {
        if (dictForKey) {
            dictForKey->retain();
        }
        if (m_levelsDict) {
            m_levelsDict->release();
        }
        m_levelsDict = dictForKey;
    }
    
    parseListLevels(dict->getStringForKey("k96"));
}

void GJLevelList::duplicateListLevels() {
    auto levels = getListLevelsArray(nullptr);
    for (int i = 0; i < levels->count(); ++i) {
        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
        addLevelToList(level);
    }
}

void GJLevelList::encodeWithCoder(DS_Dictionary* dict) {
    dict->setIntegerForKey("kCEK", 12);
    dict->setIntegerForKey("k1", m_listID);
    dict->setStringForKey("k2", m_listName);
    dict->setStringForKey("k3", m_listDesc);
    dict->setStringForKey("k5", m_creatorName);
    dict->setIntegerForKey("k60", m_accountID);
    dict->setIntegerForKey("k7", m_difficulty);
    dict->setIntegerForKey("k11", m_downloads);
    dict->setIntegerForKey("k15", m_uploaded);
    dict->setIntegerForKey("k22", m_likes);
    dict->setIntegerForKey("k21", m_listType);
    dict->setIntegerForKey("k16", m_listVersion);
    dict->setBoolForKey("k79", m_unlisted);
    dict->setBoolForKey("k94", m_friendsOnly);
    dict->setIntegerForKey("k46", m_listRevision);
    dict->setBoolForKey("k47", m_modified);
    dict->setIntegerForKey("k83", m_listOrder);
    dict->setStringForKey("k96", m_levelsString);
    dict->setBoolForKey("k82", m_favorite);
    dict->setBoolForKey("k27", m_featured);
    dict->setIntegerForKey("k42", m_original);
    dict->setIntegerForKey("k98", m_uploadDate);
    dict->setIntegerForKey("k99", m_updateDate);
    dict->setIntegerForKey("k113", m_diamonds);
    dict->setIntegerForKey("k114", m_levelsToClaim);
    dict->setBoolForKey("k100", m_onlineLevelsLoaded);
    dict->setDictForKey("k97", m_levelsDict);
}

std::string GJLevelList::frameForListDifficulty(int diff, DifficultyIconType type) {
    if (diff == 0) {
        if (type == NoText) {
            return "diffIcon_auto_btn_001.png";
        } else {
            return "difficulty_auto_btn_001.png";
        }
    }
    
    if (diff <= 10) {
        if (diff >= 6) {
            if (diff > 7) {
                if (diff == 8) {
                    diff = 6;
                }
            } else {
                ++diff;
            }
        }
        
        if (type != NoText) {
            if (diff > 5 && type == DefaultText) {
                return cocos2d::CCString::createWithFormat("difficulty_%02d_btn2_001.png", diff)->getCString();
            } else {
                return cocos2d::CCString::createWithFormat("difficulty_%02d_btn_001.png", diff)->getCString();
            }
        } else {
            return cocos2d::CCString::createWithFormat("diffIcon_%02d_btn_001.png", diff)->getCString();
        }
    } else {
        diff = 0;
        if (type != NoText) {
            return cocos2d::CCString::createWithFormat("difficulty_%02d_btn_001.png", diff)->getCString();
        } else {
            return cocos2d::CCString::createWithFormat("diffIcon_%02d_btn_001.png", diff)->getCString();
        }
    }
}

cocos2d::CCArray* GJLevelList::getListLevelsArray(cocos2d::CCArray* levels) {
    std::unordered_map<int, GJGameLevel*> levelMap;
    
    if (levels) {
        for (int i = 0; i < levels->count(); ++i) {
            auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
            int id = level->m_levelID.random - level->m_levelID.seed;
            levelMap[id] = level;
        }
    }
    
    auto result = cocos2d::CCArray::create();
    int position = 1;
    
    for (int id : m_levels) {
        auto it = levelMap.find(id);
        if (it != levelMap.end()) {
            auto level = it->second;
            m_levelsDict->setObject(GJGameLevel::getListSnapshot(level), GameToolbox::intToString(id));
        }
        
        auto savedLevel = GameLevelManager::sharedState()->getSavedLevel(id);
        auto dictLevel = static_cast<GJGameLevel*>(m_levelsDict->objectForKey(GameToolbox::intToString(id)));
        
        if (savedLevel) {
            if (savedLevel->m_onlineLevelLoaded) {
                if (!dictLevel) {
                    dictLevel = savedLevel;
                }
            } else {
                dictLevel = savedLevel;
            }
        }
        
        if (!dictLevel) {
            auto newLevel = GJGameLevel::create();
            newLevel->m_levelID = id;
            newLevel->m_downloads = static_cast<int>((rand() * 4.6566e-10f) * 1000.0f);
            newLevel->m_uploaded = id + newLevel->m_downloads;
            dictLevel = newLevel;
        }
        
        dictLevel->m_listOrder = position++;
        result->addObject(dictLevel);
    }
    
    return result;
}

std::string GJLevelList::getUnpackedDescription() {
    if (!m_listDesc.empty()) {
        return LevelTools::base64DecodeString(m_listDesc);
    }
    return "";
}

void GJLevelList::handleStatsConflict() {
    if (m_listOrder < m_listOrder) {
    }
}

bool GJLevelList::hasMatchingLevels() {
    if (m_levels.size() != 0) {
        return false;
    }
    return true;
}

bool GJLevelList::init() {
    if (!CCNode::init()) {
        return false;
    }
    
    auto dict = cocos2d::CCDictionary::create();
    if (dict != m_levelsDict) {
        if (dict) {
            dict->retain();
        }
        if (m_levelsDict) {
            m_levelsDict->release();
        }
        m_levelsDict = dict;
    }
    
    m_M_ID = s_nextId++;
    return true;
}

int GJLevelList::orderForLevel(int id) {
    for (size_t i = 0; i < m_levels.size(); ++i) {
        if (m_levels[i] == id) {
            return static_cast<int>(i);
        }
    }
    return 0;
}

void GJLevelList::parseListLevels(const std::string& str) {
    m_levelsString = str;
    m_levels.clear();
    
    if (str.empty()) {
        return;
    }
    
    size_t start = 0;
    size_t end = str.find(',');
    
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!token.empty()) {
            m_levels.push_back(std::stoi(token));
        }
        start = end + 1;
        end = str.find(',', start);
    }
    
    if (start < str.length()) {
        std::string token = str.substr(start);
        if (!token.empty()) {
            m_levels.push_back(std::stoi(token));
        }
    }
}

void GJLevelList::removeLevelFromList(int id) {
    for (auto it = m_levels.begin(); it != m_levels.end(); ++it) {
        if (*it == id) {
            m_levels.erase(it);
            updateLevelsString();
            break;
        }
    }
    
    m_levelsDict->removeObjectForKey(GameToolbox::intToString(id));
    m_modified = true;
}

void GJLevelList::reorderLevel(int levelID, int newPosition) {
    auto it = std::find(m_levels.begin(), m_levels.end(), levelID);
    if (it == m_levels.end()) {
        return;
    }
    
    int value = *it;
    m_levels.erase(it);
    
    if (newPosition < 0) {
        newPosition = 0;
    } else if (newPosition > static_cast<int>(m_levels.size())) {
        newPosition = static_cast<int>(m_levels.size());
    }
    
    m_levels.insert(m_levels.begin() + newPosition, value);
    updateLevelsString();
}

void GJLevelList::reorderLevelStep(int id, bool up) {
    int position = orderForLevel(id);
    reorderLevel(id, position + (up ? -1 : 1));
}

void GJLevelList::showListInfo() {
    std::string info = m_listDesc.empty() ? "(No description provided)" : m_listDesc;
    
    if (m_listType != Editor && (m_listID > 0 || m_uploadDate > 0)) {
        info += "\n\n";
        
        if (m_listID > 0) {
            info += "List ID: <cy>" + GameToolbox::intToString(m_listID) + "</c>\n";
        }
        
        if (m_uploadDate > 0) {
            info += "Uploaded: <cy>" + GameToolbox::timestampToHumanReadable(m_uploadDate) + "</c>";
            
            if (m_updateDate > 0 && m_updateDate != m_uploadDate) {
                info += "\nUpdated: <cy>" + GameToolbox::timestampToHumanReadable(m_updateDate) + "</c>";
            }
        }
    }
    
    auto alert = FLAlertLayer::create(nullptr, "List Info", info);
    alert->show();
}

int GJLevelList::totalLevels() {
    return static_cast<int>(m_levels.size());
}

void GJLevelList::updateLevelsString() {
    if (m_levels.empty()) {
        m_levelsString = "";
        return;
    }
    
    std::string result;
    for (size_t i = 0; i < m_levels.size(); ++i) {
        if (i > 0) {
            result += ',';
        }
        result += std::to_string(m_levels[i]);
    }
    m_levelsString = result;
}

GJLevelList::~GJLevelList() {
    if (m_levelsDict) {
        m_levelsDict->release();
    }
}