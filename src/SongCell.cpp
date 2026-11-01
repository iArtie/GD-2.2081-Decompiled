#include "SongCell.h"
#include "SongObject.h"
#include "LevelTools.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "SongInfoLayer.h"

SongCell::SongCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void SongCell::draw() {
    cocos2d::ccDrawColor4B(0, 0, 0, 0x4B);
    glLineWidth(1.0f);
    cocos2d::CCPoint p1(1.0f, m_height);
    cocos2d::CCPoint p2(m_width - 1.0f, m_height);
    cocos2d::ccDrawLine(p1, p2);
    cocos2d::CCPoint p3(1.0f, 0.0f);
    cocos2d::CCPoint p4(m_width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(p3, p4);
}

bool SongCell::init() {
    m_songObject = nullptr;
    return true;
}

void SongCell::loadFromObject(SongObject* object) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_songObject = object;

    int artistID = LevelTools::artistForAudio(object->m_audioID);
    std::string title = LevelTools::getAudioTitle(object->m_audioID);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    titleLabel->setScale(1.0f);
    titleLabel->setPosition(cocos2d::CCPoint(10.0f, m_height * 0.5f + 10.0f));
    if (titleLabel->getContentSize().width > 240.0f)
        titleLabel->setScale(240.0f / titleLabel->getContentSize().width);
    if (titleLabel->getScale() > 0.7f)
        titleLabel->setScale(0.7f);

    ButtonSprite* viewSprite = ButtonSprite::create("View", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* viewBtn = CCMenuItemSpriteExtra::create(viewSprite, nullptr, menu_selector(SongCell::onClick));
    viewBtn->setSizeMult(1.5f);
    cocos2d::CCMenu* buttonMenu = cocos2d::CCMenu::create(viewBtn, nullptr);
    m_mainLayer->addChild(buttonMenu);
    buttonMenu->setPosition(cocos2d::CCPoint(
        m_width - (viewSprite->getContentSize().width * 0.5f) - 9.0f,
        m_height * 0.5f));

    std::string artistName = LevelTools::nameForArtist(artistID);
    std::string artistStr = cocos2d::CCString::createWithFormat("By %s", artistName.c_str())->getCString();

    cocos2d::CCLabelBMFont* artistLabel = cocos2d::CCLabelBMFont::create(artistStr.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(artistLabel);
    artistLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    artistLabel->setScale(1.0f);
    artistLabel->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(2.0f, -21.0f));
    if (artistLabel->getContentSize().width > 140.0f)
        artistLabel->setScale(140.0f / artistLabel->getContentSize().width);
    if (artistLabel->getScale() > 0.7f)
        artistLabel->setScale(0.7f);
}

void SongCell::onClick(cocos2d::CCObject* sender) {
    SongInfoLayer* layer = SongInfoLayer::create(m_songObject->m_audioID);
    layer->show();
}

void SongCell::updateBGColor(int index) {
    if (index & 1)
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    else
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    m_backgroundLayer->setOpacity(255);
}

SongCell::~SongCell() {
}