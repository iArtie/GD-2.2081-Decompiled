#pragma once

#include "cocos2d.h"
#include <string>

class CCURLObject : public cocos2d::CCObject {
public:
    static CCURLObject* create(std::string name, std::string url);

    bool init(std::string name, std::string url);

    std::string getName() const { return m_name; }
    std::string getUrl() const { return m_url; }
    void setName(const std::string& name) { m_name = name; }
    void setUrl(const std::string& url) { m_url = url; }

    ~CCURLObject();

private:
    std::string m_name;
    std::string m_url;
};