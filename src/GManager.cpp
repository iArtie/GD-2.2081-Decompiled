#include "GManager.h"
#include "DS_Dictionary.h"
#include "cocos2d.h"

bool GManager::init() {
    m_setup = true;
    m_saved = false;
    return true;
}

void GManager::setup() {
    m_setup = true;
    load();
}

void GManager::load() {
    loadDataFromFile(m_fileName);
}

void GManager::loadDataFromFile(const std::string& filename) {
    auto dict = new DS_Dictionary();
    
    bool loaded = false;
    
    if (tryLoadData(dict, &filename)) {
        loaded = true;
    } else {
        std::string oldFile = filename + "2.dat";
        if (tryLoadData(dict, &oldFile)) {
            loaded = true;
            dict->copyFile(oldFile.c_str(), filename.c_str());
        } else {
            std::string backupFile = filename + ".dat.bak";
            if (tryLoadData(dict, &backupFile)) {
                loaded = true;
            }
        }
    }
    
    if (loaded) {
        dataLoaded(dict);
    } else {
        firstLoad();
    }
    
    delete dict;
}

bool GManager::tryLoadData(DS_Dictionary* dict, const std::string* filename) {
    return dict->loadRootSubDictFromCompressedFile(filename->c_str());
}

void GManager::loadFromString(std::string* str) {
    auto dict = new DS_Dictionary();
    
    if (dict->loadRootSubDictFromString(str)) {
        dataLoaded(dict);
    }
    
    delete dict;
}

void GManager::loadFromCompressedString(std::string* str) {
    cocos2d::ZipUtils::decompressString(str);
    str->resize(str->length());
    loadFromString(str);
}

void GManager::save() {
    if (!m_saved) {
        m_saved = true;
        saveGMTo(m_fileName);
    }
}

void GManager::saveGMTo(const std::string& filename) {
    auto dict = new DS_Dictionary();
    encodeDataTo(dict);
    saveData(dict, filename);
    m_saved = false;
    delete dict;
}

void GManager::saveData(DS_Dictionary* dict, const std::string& filename) {
    dict->saveRootSubDictToCompressedFile(filename.c_str());
}

std::string GManager::getSaveString() {
    auto dict = new DS_Dictionary();
    encodeDataTo(dict);
    std::string result = dict->saveRootSubDictToString();
    delete dict;
    return result;
}

std::string GManager::getCompressedSaveString() {
    std::string saveString = getSaveString();
    std::string compressed;
    cocos2d::ZipUtils::compressString(&saveString, &compressed);
    return compressed;
}

void GManager::dataLoaded(DS_Dictionary* dict) {
}

void GManager::encodeDataTo(DS_Dictionary* dict) {
}

void GManager::firstLoad() {
}

GManager::~GManager() {
}