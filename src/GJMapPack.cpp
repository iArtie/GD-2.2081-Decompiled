#include "GJMapPack.h"
#include "GameStatsManager.h"

int GJMapPack::s_nextId = 0;

int GJMapPack::completedMaps() {
    if (!m_levels) {
        return 0;
    }
    
    int completed = 0;
    auto stats = GameStatsManager::sharedState();
    
    for (int i = 0; i < m_levels->count(); ++i) {
        auto str = static_cast<cocos2d::CCString*>(m_levels->objectAtIndex(i));
        int levelId = str->intValue();
        
        if (m_isGauntlet) {
            if (stats->hasCompletedGauntletLevel(levelId)) {
                ++completed;
            }
        } else {
            if (stats->hasCompletedOnlineLevel(levelId)) {
                ++completed;
            }
        }
    }
    
    return completed;
}

GJMapPack* GJMapPack::create() {
    auto pack = new GJMapPack();
    if (pack && pack->init()) {
        pack->autorelease();
        return pack;
    }
    delete pack;
    return nullptr;
}

bool GJMapPack::hasCompletedMapPack() {
    int total = totalMaps();
    if (total == 0) {
        return false;
    }
    return completedMaps() >= total;
}

bool GJMapPack::init() {
    m_packName = "";
    m_levels = nullptr;
    m_stars = 0;
    m_coins = 0;
    m_packID = 0;
    m_MId = s_nextId++;
    m_isGauntlet = false;
    m_textColour = cocos2d::ccc3(255, 255, 255);
    m_barColour = cocos2d::ccc3(255, 255, 255);
    return true;
}

void GJMapPack::parsePackColors(const std::string& textColorStr, const std::string& barColorStr) {
    m_textColour = cocos2d::ccc3(255, 255, 255);
    m_barColour = cocos2d::ccc3(255, 255, 255);
    
    auto textColors = cocos2d::CCArray::create();
    auto barColors = cocos2d::CCArray::create();
    
    std::string textStr = textColorStr;
    size_t pos = 0;
    while ((pos = textStr.find(',')) != std::string::npos) {
        textColors->addObject(cocos2d::CCString::create(textStr.substr(0, pos)));
        textStr.erase(0, pos + 1);
    }
    if (!textStr.empty()) {
        textColors->addObject(cocos2d::CCString::create(textStr));
    }
    
    std::string barStr = barColorStr;
    pos = 0;
    while ((pos = barStr.find(',')) != std::string::npos) {
        barColors->addObject(cocos2d::CCString::create(barStr.substr(0, pos)));
        barStr.erase(0, pos + 1);
    }
    if (!barStr.empty()) {
        barColors->addObject(cocos2d::CCString::create(barStr));
    }
    
    if (textColors->count() > 2) {
        auto r = static_cast<cocos2d::CCString*>(textColors->objectAtIndex(0))->intValue();
        auto g = static_cast<cocos2d::CCString*>(textColors->objectAtIndex(1))->intValue();
        auto b = static_cast<cocos2d::CCString*>(textColors->objectAtIndex(2))->intValue();
        m_textColour = cocos2d::ccc3(r, g, b);
    }
    
    if (barColors->count() > 2) {
        auto r = static_cast<cocos2d::CCString*>(barColors->objectAtIndex(0))->intValue();
        auto g = static_cast<cocos2d::CCString*>(barColors->objectAtIndex(1))->intValue();
        auto b = static_cast<cocos2d::CCString*>(barColors->objectAtIndex(2))->intValue();
        m_barColour = cocos2d::ccc3(r, g, b);
    }
}

void GJMapPack::parsePackLevels(const std::string& levelsStr) {
    if (m_levels) {
        m_levels->release();
        m_levels = nullptr;
    }
    
    m_levels = cocos2d::CCArray::create();
    m_levels->retain();
    
    std::string str = levelsStr;
    size_t pos = 0;
    while ((pos = str.find(',')) != std::string::npos) {
        m_levels->addObject(cocos2d::CCString::create(str.substr(0, pos)));
        str.erase(0, pos + 1);
    }
    if (!str.empty()) {
        m_levels->addObject(cocos2d::CCString::create(str));
    }
}

int GJMapPack::totalMaps() {
    return m_levels ? m_levels->count() : 0;
}

GJMapPack::~GJMapPack() {
    if (m_levels) {
        m_levels->release();
    }
}