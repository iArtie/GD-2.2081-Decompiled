#include "Cells/StatsCell.h"
#include "Models/StatsObject.h"

StatsCell::StatsCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void StatsCell::draw() {
    cocos2d::ccDrawColor4B(0, 0, 0, 0x4B);
    glLineWidth(1.0f);
    cocos2d::CCPoint p1(1.0f, m_height);
    cocos2d::CCPoint p2(m_width - 1.0f, m_height);
    cocos2d::ccDrawLine(p1, p2);
    cocos2d::CCPoint p3(1.0f, 0.0f);
    cocos2d::CCPoint p4(m_width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(p3, p4);
}

const char* StatsCell::getTitleFromKey(const char* key) {
    std::string k(key);
    if (k == "1") return "Total Jumps:";
    if (k == "2") return "Total Attempts:";
    if (k == "3") return "Completed Levels:";
    if (k == "4") return "Completed Online Levels:";
    if (k == "15") return "Completed Daily Levels:";
    if (k == "5") return "Completed Demon Levels:";
    if (k == "6") return "Collected Stars:";
    if (k == "28") return "Collected Moons:";
    if (k == "8") return "Collected Secret Coins:";
    if (k == "12") return "Collected User Coins:";
    if (k == "7") return "Completed Map Packs:";
    if (k == "40") return "Completed Gauntlets:";
    if (k == "9") return "Players destroyed:";
    if (k == "10") return "Liked/Disliked Levels:";
    if (k == "11") return "Rated Levels:";
    if (k == "14") return "Total orbs collected:";
    if (k == "13") return "Collected Diamonds:";
    if (k == "41") return "Collected List Rewards:";
    if (k == "42") return "Completed Insane Levels:";
    return key;
}

bool StatsCell::init() {
    return true;
}

void StatsCell::loadFromObject(StatsObject* object) {
    m_backgroundLayer->setOpacity(255);
    m_mainLayer->removeAllChildrenWithCleanup(true);

    const char* title = getTitleFromKey(object->m_key.c_str());
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(title, "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    if (titleLabel->getContentSize().width > 200.0f) {
        float scale = 200.0f / titleLabel->getContentSize().width;
        if (scale < 1.0f)
            titleLabel->setScale(scale);
    }

    cocos2d::CCLabelBMFont* valueLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i", object->m_value)->getCString(), "bigFont.fnt");
    m_mainLayer->addChild(valueLabel);
    if (valueLabel->getContentSize().width > 100.0f) {
        float scale = 100.0f / valueLabel->getContentSize().width;
        if (scale < 1.0f)
            valueLabel->setScale(scale);
    }

    if (titleLabel->getScale() > 0.7f)
        titleLabel->setScale(0.7f);
    if (valueLabel->getScale() > 0.7f)
        valueLabel->setScale(0.7f);

    titleLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    valueLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    titleLabel->setPosition(cocos2d::CCPoint(20.0f, m_height * 0.5f - 1.0f));
    valueLabel->setPosition(cocos2d::CCPoint(330.0f, m_height * 0.5f - 1.0f));
}

void StatsCell::updateBGColor(int index) {
    if (index & 1)
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    else
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
}

StatsCell::~StatsCell() {
}