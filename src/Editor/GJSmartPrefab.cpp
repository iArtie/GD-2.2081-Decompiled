#include "Editor/GJSmartPrefab.h"
#include "Utils/DS_Dictionary.h"

static int s_nextPrefabId = 0;

GJSmartPrefab* GJSmartPrefab::create() {
    auto prefab = new GJSmartPrefab();
    if (prefab && prefab->init()) {
        prefab->autorelease();
        return prefab;
    }
    delete prefab;
    return nullptr;
}

GJSmartPrefab* GJSmartPrefab::createWithCoder(DS_Dictionary* dict) {
    auto prefab = GJSmartPrefab::create();
    prefab->dataLoaded(dict);
    return prefab;
}

bool GJSmartPrefab::canEncode() {
    return true;
}

void GJSmartPrefab::dataLoaded(DS_Dictionary* dict) {
    m_prefabData = dict->getStringForKey("1");
    m_prefabChance = dict->getIntegerForKey("2");
}

void GJSmartPrefab::encodeWithCoder(DS_Dictionary* dict) {
    dict->setIntegerForKey("kCEK", 11);
    dict->setStringForKey("1", m_prefabData);
    dict->setIntegerForKey("2", m_prefabChance);
}

bool GJSmartPrefab::init() {
    m_prefabID = s_nextPrefabId++;
    m_prefabChance = 0;
    return true;
}

GJSmartPrefab::~GJSmartPrefab() {
}