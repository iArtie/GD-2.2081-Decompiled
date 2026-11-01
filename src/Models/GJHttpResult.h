#pragma once

#include <string>
#include "Platform/cocos2d.h"

enum class GJHttpType {
    Unknown = 0
};

class GJHttpResult : public cocos2d::CCNode {
public:
    static GJHttpResult* create(bool success, const std::string& response, 
                               const std::string& tag, GJHttpType type);
    bool init(bool success, const std::string& response, 
              const std::string& tag, GJHttpType type);
    virtual ~GJHttpResult();

    bool isSuccess() const { return m_success; }
    const std::string& getResponse() const { return m_response; }
    const std::string& getRequestTag() const { return m_requestTag; }
    GJHttpType getHttpType() const { return m_httpType; }

private:
    bool m_success;
    std::string m_response;
    std::string m_requestTag;
    GJHttpType m_httpType;
};