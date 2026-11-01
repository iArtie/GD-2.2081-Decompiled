#include "Cells/SmartTemplateCell.h"
#include "Editor/GJSmartTemplate.h"
#include "GameLevelManager.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

SmartTemplateCell::SmartTemplateCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void SmartTemplateCell::draw() {
    cocos2d::ccDrawColor4B(0, 0, 0, 0x4B);
    glLineWidth(1.0f);
    cocos2d::CCPoint p1(1.0f, m_height);
    cocos2d::CCPoint p2(m_width - 1.0f, m_height);
    cocos2d::ccDrawLine(p1, p2);
    cocos2d::CCPoint p3(1.0f, 0.0f);
    cocos2d::CCPoint p4(m_width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(p3, p4);
}

bool SmartTemplateCell::init() {
    m_smartTemplate = nullptr;
    return true;
}

void SmartTemplateCell::loadFromObject(GJSmartTemplate* smartTemplate) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_smartTemplate = smartTemplate;

    cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create(smartTemplate->m_templateName.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(nameLabel);
    nameLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    nameLabel->setPosition(cocos2d::CCPoint(12.0f, m_height * 0.5f));
    nameLabel->limitLabelWidth(200.0f, 0.8f, 0.0f);

    const char* bgImage;
    GJSmartTemplate* currentTemplate = GameLevelManager::sharedState()->m_smartTemplate;
    if (currentTemplate && currentTemplate->m_nameIndex == smartTemplate->m_nameIndex)
        bgImage = "GJ_button_02.png";
    else
        bgImage = "GJ_button_01.png";

    ButtonSprite* useSprite = ButtonSprite::create("Use", 0, 0, 0.5f, true, "bigFont.fnt", bgImage, 30.0f);
    CCMenuItemSpriteExtra* useBtn = CCMenuItemSpriteExtra::create(useSprite, nullptr, menu_selector(SmartTemplateCell::onClick));
    useBtn->setSizeMult(1.5f);
    useBtn->setTag(0);

    ButtonSprite* editSprite = ButtonSprite::create("Edit", 0, 0, 0.5f, true, "bigFont.fnt", "GJ_button_03.png", 30.0f);
    CCMenuItemSpriteExtra* editBtn = CCMenuItemSpriteExtra::create(editSprite, nullptr, menu_selector(SmartTemplateCell::onClick));
    editBtn->setSizeMult(1.5f);
    editBtn->setTag(1);

    cocos2d::CCMenu* buttonMenu = cocos2d::CCMenu::create(useBtn, editBtn, nullptr);
    cocos2d::CCPoint menuPos(m_width - 55.0f - 25.0f - 10.0f, m_height * 0.5f);
    cocos2d::CCPoint nodePos = cocos2d::CCNode::convertToNodeSpace(&menuPos);
    useBtn->setPosition(nodePos);
    editBtn->setPosition(nodePos + cocos2d::CCPoint(55.0f, 0.0f));
    m_mainLayer->addChild(buttonMenu);
}

void SmartTemplateCell::onClick(cocos2d::CCObject* sender) {
    if (!sender)
        return;

    int tag = sender->getTag();
    if (tag == 0) {
        if (m_smartTemplate) {
            GameLevelManager::sharedState()->m_smartTemplate = m_smartTemplate;
            if (m_tableView->m_cellDelegate)
                m_tableView->m_cellDelegate->cellPerformedAction(this, 22, CellAction::Click, 0);
        }
    } else if (tag == 1) {
        if (m_tableView->m_cellDelegate)
            m_tableView->m_cellDelegate->cellPerformedAction(this, 22, CellAction::Edit, 0);
    }
}

void SmartTemplateCell::updateBGColor(int index) {
    GJSmartTemplate* currentTemplate = GameLevelManager::sharedState()->m_smartTemplate;
    if (currentTemplate && currentTemplate->m_nameIndex == m_smartTemplate->m_nameIndex) {
        m_backgroundLayer->setColor(cocos2d::ccc3(230, 150, 10));
    } else {
        if (index & 1)
            m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
        else
            m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

SmartTemplateCell::~SmartTemplateCell() {
}