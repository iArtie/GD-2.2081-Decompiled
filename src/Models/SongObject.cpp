#include "Models/SongObject.h"

SongObject* SongObject::create(int audioID) {
    SongObject* returnVal = new SongObject();
    if (returnVal->init(audioID)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool SongObject::init(int audioID) {
    m_audioID = audioID;
    return true;
}

SongObject::~SongObject() {
}