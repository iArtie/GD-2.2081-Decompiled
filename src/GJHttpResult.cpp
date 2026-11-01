#include "GJHttpResult.h"

GJHttpResult* GJHttpResult::create(bool success, const std::string& response, 
                                   const std::string& tag, GJHttpType type) {
    auto result = new GJHttpResult();
    if (result && result->init(success, response, tag, type)) {
        result->autorelease();
        return result;
    }
    delete result;
    return nullptr;
}

bool GJHttpResult::init(bool success, const std::string& response, 
                        const std::string& tag, GJHttpType type) {
    if (!CCNode::init()) {
        return false;
    }
    
    m_success = success;
    m_response = response;
    m_requestTag = tag;
    m_httpType = type;
    
    return true;
}

GJHttpResult::~GJHttpResult() {
}