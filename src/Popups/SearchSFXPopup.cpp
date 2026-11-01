#include "Popups/SearchSFXPopup.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"

SearchSFXPopup* SearchSFXPopup::create(std::string query)
{
    SearchSFXPopup* returnVal = new SearchSFXPopup();

    if (returnVal->init(query))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool SearchSFXPopup::init(std::string query)
{
    if (!SetTextPopup::init(query, "Search", 20, "Find SFX", "OK", true, 60.0f))
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    bool searchFolders = GameManager::sharedState()->getGameVariable("0147");

    cocos2d::CCPoint position(
        winSize.width * 0.5f - 124.0f,
        winSize.height * 0.5f + 10.0f);

    cocos2d::CCPoint labelOffset(8.0f, 0.0f);

    GameToolbox::createToggleButton(
        "Folder\nName",
        menu_selector(SearchSFXPopup::onSearchFolders),
        0,
        searchFolders,
        m_buttonMenu,
        position,
        this,
        m_mainLayer,
        0.8f,
        0.3f,
        0.0f,
        labelOffset,
        "bigFont.fnt",
        false,
        false,
        nullptr);

    return true;
}

void SearchSFXPopup::onSearchFolders(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->toggleGameVariable("0147");
}

SearchSFXPopup::~SearchSFXPopup()
{
}