#include "SimpleObject.h"

SimpleObject* SimpleObject::create() {
    SimpleObject* returnVal = new SimpleObject();
    if (returnVal->init()) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool SimpleObject::init() {
    return true;
}

SimpleObject::~SimpleObject() {
}