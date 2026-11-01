#pragma once

#include "cocos2d.h"
#include <string>

class CommunityCreditNode : public cocos2d::CCNode {
public:
    static CommunityCreditNode* create(int unlockType, int iconID, int unknown, std::string author);

    bool init(int unlockType, int iconID, int unknown, std::string author);

    int getUnlockType() const { return m_unlockType; }
    int getIconID() const { return m_iconID; }
    int getUnknown() const { return m_unknown; }
    std::string getAuthor() const { return m_author; }

    ~CommunityCreditNode();

private:
    int m_unlockType;
    int m_iconID;
    int m_unknown;
    std::string m_author;
};