#include "OptionsCell.h"
#include "OptionsObject.h"
#include "GameToolbox.h"
#include "CCMenuItemSpriteExtra.h"
#include "TableView.h"
#include "TableViewCellDelegate.h"

OptionsCell::OptionsCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height)
{
    m_optionsObject = nullptr;
    cocos2d::CCLayer::init();
}

void OptionsCell::loadFromObject(OptionsObject* object)
{
    m_optionsObject = object;
    m_mainLayer->removeAllChildrenWithCleanup(true);

    int count = object->m_count;

    cocos2d::CCLabelBMFont* countLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(countLabel, 2);
    countLabel->setScale(0.4f);
    countLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    countLabel->setPosition(cocos2d::CCPoint(40.0f, m_height * 0.5f));

    if (count)
    {
        if (count > 0)
        {
            std::string countStr = "[ " + GameToolbox::intToString(count) + " ]";

            cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create(object->m_name.c_str(), "bigFont.fnt");
            m_mainLayer->addChild(nameLabel, 2);
            nameLabel->setScale(0.6f);
            nameLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
            nameLabel->setColor(cocos2d::ccc3(255, 255, 255));
            nameLabel->setOpacity(255);

            float xPos = countLabel->getPosition().x + countLabel->getContentSize().width * countLabel->getScale() + 8.0f;
            nameLabel->setPosition(cocos2d::CCPoint(xPos, countLabel->getPosition().y));
        }
    }
    else
    {
        countLabel->setColor(cocos2d::ccc3(255, 255, 255));
    }

    const char* spriteName = m_optionsObject->m_enabled ? "GJ_checkOn_001.png" : "GJ_checkOff_001.png";
    cocos2d::CCSprite* checkSprite = cocos2d::CCSprite::createWithSpriteFrameName(spriteName);
    checkSprite->setScale(0.6f);

    CCMenuItemSpriteExtra* checkBtn = CCMenuItemSpriteExtra::create(
        checkSprite, nullptr, menu_selector(OptionsCell::onToggleOption));
    checkBtn->setSizeMult(1.5f);

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(checkBtn, nullptr);
    m_mainLayer->addChild(menu, 3);
    menu->setPosition(cocos2d::CCPoint(20.0f, m_height * 0.5f));
}

void OptionsCell::onToggleOption(cocos2d::CCObject* sender)
{
    if (m_optionsObject)
    {
        m_optionsObject->toggleState();

        if (m_tableView)
        {
            TableViewCellDelegate* delegate = m_tableView->m_cellDelegate;
            if (delegate)
                delegate->cellPerformedAction(this, 26, CellAction::Click, 0);
        }
    }
}

void OptionsCell::updateBGColor(int index)
{
    if ((index & 1) != 0)
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    else
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));

    m_backgroundLayer->setOpacity(255);
}

OptionsCell::~OptionsCell()
{
}