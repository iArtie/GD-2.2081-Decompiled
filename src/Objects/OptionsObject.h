#pragma once

#include <string>
#include "Platform/cocos2d.h"

class OptionsObjectDelegate;

class OptionsObject : public cocos2d::CCObject {
public:
    static OptionsObject* create(int id, bool enabled, std::string name, OptionsObjectDelegate* delegate);

    bool init(int id, bool enabled, std::string name, OptionsObjectDelegate* delegate);
    void toggleState();

    virtual ~OptionsObject();

    int m_optionID = 0;
    bool m_enabled = false;
    std::string m_name;
    OptionsObjectDelegate* m_delegate = nullptr;
    int m_count = 0;
};