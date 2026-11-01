#include "Cocos/CCURLObject.h"
#include "Platform/cocos2d.h"

CCURLObject* CCURLObject::create(std::string name, std::string url) {
    CCURLObject* obj = new CCURLObject();
    if (obj && obj->init(name, url)) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool CCURLObject::init(std::string name, std::string url) {
    m_name = name;
    m_url = url;
    return true;
}

CCURLObject::~CCURLObject() {
    // Destructor
}