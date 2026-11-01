#include "StatsObject.h"

StatsObject* StatsObject::create(const char* key, int value) {
    StatsObject* returnVal = new StatsObject();
    if (returnVal->init(key, value)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool StatsObject::init(const char* key, int value) {
    m_key = key;
    m_value = value;
    return true;
}

StatsObject::~StatsObject() {
}