#include "GJLocalLevelScoreCell.h"
#include "GJLocalScore.h"
#include "GameToolbox.h"

GJLocalLevelScoreCell::GJLocalLevelScoreCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void GJLocalLevelScoreCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

bool GJLocalLevelScoreCell::init() {
    m_localScore = nullptr;
    return true;
}

void GJLocalLevelScoreCell::loadFromScore(GJLocalScore* score) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_localScore = score;
    
    std::string scoreText;
    if (m_localScore->m_type) {
        scoreText = GameToolbox::pointsToString(m_localScore->m_points);
    } else {
        scoreText = GameToolbox::msToTimeString(m_localScore->m_points, 1);
    }
    
    const char* iconFrame = m_localScore->m_type ? "GJ_pointsIcon_001.png" : "GJ_timeIcon_001.png";
    
    auto icon = cocos2d::CCSprite::createWithSpriteFrameName(iconFrame);
    icon->setScale(0.55f);
    m_mainLayer->addChild(icon, 20);
    icon->setPosition(cocos2d::CCPoint(20.0f, m_height * 0.5f));
    
    auto label = cocos2d::CCLabelBMFont::create(scoreText.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(label, 2);
    label->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    label->limitLabelWidth(200.0f, 0.46f, 0.0f);
    label->setPosition(cocos2d::CCPoint(40.0f, m_height * 0.5f + 0.5f));
}

void GJLocalLevelScoreCell::updateBGColor(int index) {
    if (m_localScore && m_localScore->m_currentScore) {
        m_backgroundLayer->setColor(cocos2d::ccc3(230, 150, 10));
    } else {
        if (index & 1) {
            m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
        } else {
            m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
        }
    }
    m_backgroundLayer->setOpacity(255);
}

GJLocalLevelScoreCell::~GJLocalLevelScoreCell() {
}