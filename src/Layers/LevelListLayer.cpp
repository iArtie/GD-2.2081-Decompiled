#include "Layers/LevelListLayer.h"
#include "Models/GJLevelList.h"
#include "Models/GJSearchObject.h"
#include "Layers/GJListLayer.h"
#include "Managers/GJAccountManager.h"
#include "Models/GJGameLevel.h"
#include "Managers/GameManager.h"
#include "GameLevelManager.h"
#include "Managers/GameStatsManager.h"
#include "Utils/GameToolbox.h"
#include "Layers/LevelBrowserLayer.h"
#include "Delegates/LevelListDeleteDelegate.h"
#include "Delegates/LikeItemDelegate.h"
#include "Layers/LikeItemLayer.h"
#include "Layers/SelectListIconLayer.h"
#include "Layers/ShareListLayer.h"
#include "Layers/ShareCommentLayer.h"
#include "Popups/SetIDPopup.h"
#include "Popups/SetFolderPopup.h"
#include "Layers/FLAlertLayer.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/LoadingCircle.h"
#include "UI/TextArea.h"
#include "Cocos/CCTextInputNode.h"
#include "Layers/InfoLayer.h"
#include "Pages/ProfilePage.h"
#include "UI/CustomListView.h"
#include "Layers/CurrencyRewardLayer.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Cocos/CCSpriteGrayscale.h"
#include "Cocos/CCSpriteWithHue.h"
#include "CCDirector.h"
#include "CCScene.h"
#include "CCTransitionFade.h"
#include "Audio/FMODAudioEngine.h"
#include "Utils/LevelTools.h"
#include "Utils/PlatformToolbox.h"
#include "Delegates/AppDelegate.h"
#include "cocos-ext.h"

void LevelListLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    int tag = layer->getTag();
    switch (tag) {
        case 1004:
            if (btn2) {
                ownerDelete();
            }
            break;
        case 1005:
            if (btn2) {
                cloneList();
            }
            break;
        case 1001:
            if (btn2) {
                onDelete();
            }
            break;
        default:
            LevelBrowserLayer::FLAlert_Clicked(layer, btn2);
            break;
    }
}

void LevelListLayer::cloneList() {
    if (m_exiting) {
        return;
    }

    setKeypadEnabled(false);
    m_exiting = true;
    GameManager::sharedState()->m_sceneEnum = 14;

    GJLevelList* newList = GameLevelManager::sharedState()->createNewLevelList();
    GameLevelManager::sharedState()->m_returnToLocalLevels = true;

    newList->m_difficulty = m_levelList->m_difficulty;
    newList->m_listDesc = m_levelList->m_listDesc;

    newList->duplicateListLevels();

    std::string newName = m_levelList->m_listName;
    if (newName.size() <= 23) {
        newName += " 2";
    }
    newList->m_listName = newName;

    int originalID = m_levelList->m_original;
    if (originalID == 0) {
        originalID = m_levelList->m_listID;
    }
    newList->m_original = originalID;

    cocos2d::CCScene* scene = LevelListLayer::scene(newList);
    cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
}

void LevelListLayer::confirmClone(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }
    FLAlertLayer* alert = FLAlertLayer::create(this, "Clone List", "Create a <cl>copy</c> of this <cl>list</c>?");
    alert->setTag(1005);
    alert->show();
}

void LevelListLayer::confirmDelete(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }
    FLAlertLayer* alert = FLAlertLayer::create(this, "Delete List", "Are you sure you want to <cr>delete</c> this list?");
    alert->setTag(1001);
    alert->show();
}

void LevelListLayer::confirmOwnerDelete(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }
    FLAlertLayer* alert = FLAlertLayer::create(this, "Remove Server List",
        "Do you want to <cr>delete</c> this <cg>list</c> from the <cl>server</c>? <cy>(Your list will no longer be available online)</c>");
    alert->setTag(1004);
    alert->show();
}

LevelListLayer* LevelListLayer::create(GJLevelList* list) {
    LevelListLayer* layer = new LevelListLayer();
    if (layer && layer->init(list)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void LevelListLayer::iconSelectClosed(SelectListIconLayer* layer) {
    m_levelList->m_difficulty = layer->m_currentDifficulty;
    updateStatsArt();
}

bool LevelListLayer::init(GJLevelList* list) {
    cocos2d::ccColor4B color = { 0, 0, 0, 0 };
    if (!cocos2d::CCLayerColor::initWithColor(color)) {
        return false;
    }

    m_editMode = 0;

    m_searchKey1 = GameLevelManager::sharedState()->m_searchKey;

    m_levelList = list;
    m_levelList->retain();

    m_objects = cocos2d::CCArray::create();
    m_objects->retain();

    GJSearchObject* searchObject = GJSearchObject::create(SearchType::LevelListsOnClick);
    bool isEditor = (m_levelList->m_listType == GJLevelType::Editor);

    if (!isEditor) {
        searchObject->m_searchQuery = GameToolbox::intToString(list->m_listID);
    } else {
        m_listHeight = 200;
        m_unkFloat = -9.0f;
        searchObject->m_searchType = SearchType::MyLists;
        searchObject->m_searchMode = 31;
        searchObject->m_searchQuery = list->m_levelsString;

        if (!m_levelList->m_listName.empty()) {
            m_levelList->m_unkString = m_levelList->m_listName;
        }
    }

    setSearchObject(searchObject);

    GameLevelManager::sharedState()->m_searchType = searchObject->m_searchType;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = director->getWinSize();

    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bg, -2);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().height);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().width);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(102, 102, 255));

    cocos2d::CCSprite* sideArtLeft = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtLeft->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    sideArtLeft->setPosition(cocos2d::CCPoint(director->getScreenLeft() - 1.0f, director->getScreenBottom() - 1.0f));
    addChild(sideArtLeft, 1);

    cocos2d::CCSprite* sideArtRight = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtRight->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    sideArtRight->setPosition(cocos2d::CCPoint(director->getScreenRight() + 1.0f, director->getScreenBottom() - 1.0f));
    sideArtRight->setFlipX(true);
    addChild(sideArtRight, 1);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(closeSprite, nullptr, this,
        menu_selector(LevelListLayer::onBack));
    closeButton->setSizeMult(1.6f);

    cocos2d::CCMenu* closeMenu = cocos2d::CCMenu::create(closeButton, nullptr);
    m_buttonMenu = closeMenu;
    addChild(closeMenu, 1);
    closeMenu->setPosition(cocos2d::CCPoint(director->getScreenLeft() + 24.0f, director->getScreenTop() - 23.0f));

    std::string diffFrame = GJLevelList::frameForListDifficulty(m_levelList->m_difficulty, DifficultyIconType::NoText);
    m_diffSprite = cocos2d::CCSprite::createWithSpriteFrameName(diffFrame.c_str());
    m_featureSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");

    if (isEditor) {
        cocos2d::CCRect rect(0.0f, 0.0f, 80.0f, 80.0f);
        cocos2d::CCPoint titlePos(winSize.width * 0.5f, director->getScreenTop() - 30.0f);

        cocos2d::extension::CCScale9Sprite* titleBg = cocos2d::extension::CCScale9Sprite::create(
            "square02b_001.png", rect);
        titleBg->setOpacity(255);
        titleBg->setContentSize(cocos2d::CCSize(365.0f, 40.0f));
        addChild(titleBg, -1);
        titleBg->setPosition(titlePos);
        titleBg->setColor(cocos2d::ccc3(0, 0, 141));

        if (m_levelList->m_listVersion > 1 || m_levelList->m_uploaded) {
            cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(
                m_levelList->m_listName.c_str(), "bigFont.fnt");
            addChild(titleLabel, 20);
            titleLabel->setPosition(titlePos);
        } else {
            CCTextInputNode* input = CCTextInputNode::create(300.0f, 50.0f, "List Name", "Thonburi");
            m_titleInput = input;
            addChild(input, 18);
            m_titleInput->m_delegate = this;
            m_titleInput->setTag(1002);
            m_titleInput->setPosition(titlePos);
            m_titleInput->m_maxLabelLength = 25;
            m_titleInput->m_filterSwearWords = true;

            if (!m_levelList->m_listName.empty()) {
                m_titleInput->setString(m_levelList->m_listName);
            }

            m_titleInput->setLabelPlaceholderColor(cocos2d::ccc3(88, 88, 240));
            m_titleInput->setLabelPlaceholderScale(0.7f);
        }

        m_unk2 = true;

        cocos2d::CCPoint btnsStart(director->getScreenRight() - 25.0f, director->getScreenTop() - 25.0f);
        cocos2d::CCPoint menuPos = m_buttonMenu->convertToNodeSpace(btnsStart);

        cocos2d::CCSprite* deleteSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
        deleteSpr->setScale(0.8f);
        CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(deleteSpr, nullptr, this,
            menu_selector(LevelListLayer::confirmDelete));
        m_buttonMenu->addChild(deleteBtn);
        deleteBtn->setPosition(m_buttonMenu->convertToNodeSpace(btnsStart));

        cocos2d::CCPoint currentPos = deleteBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* refreshSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        refreshSpr->setScale(0.8f);
        m_refreshBtn = CCMenuItemSpriteExtra::create(refreshSpr, nullptr, this,
            menu_selector(LevelBrowserLayer::onRefresh));
        m_buttonMenu->addChild(m_refreshBtn);
        m_refreshBtn->setPosition(currentPos);
        currentPos = m_refreshBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* descSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
        descSpr->setScale(0.8f);
        CCMenuItemSpriteExtra* descBtn = CCMenuItemSpriteExtra::create(descSpr, nullptr, this,
            menu_selector(LevelListLayer::onDescription));
        m_buttonMenu->addChild(descBtn);
        descBtn->setPosition(currentPos);
        currentPos = descBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* editSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_editModeBtn_001.png");
        editSpr->setScale(0.8f);
        CCMenuItemSpriteExtra* editBtn = CCMenuItemSpriteExtra::create(editSpr, nullptr, this,
            menu_selector(LevelListLayer::onToggleEditMode));
        m_buttonMenu->addChild(editBtn);
        editBtn->setPosition(currentPos);
        currentPos = editBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* iconSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
        iconSpr->addChild(m_diffSprite, 3);
        iconSpr->addChild(m_featureSprite, 2);
        m_diffSprite->setPosition(cocos2d::CCPoint(
            iconSpr->getContentSize().width * 0.5f,
            iconSpr->getContentSize().height * 0.5f));
        m_featureSprite->setPosition(m_diffSprite->getPosition());
        iconSpr->setScale(0.8f);
        CCMenuItemSpriteExtra* iconBtn = CCMenuItemSpriteExtra::create(iconSpr, nullptr, this,
            menu_selector(LevelListLayer::onSelectIcon));
        m_buttonMenu->addChild(iconBtn);
        iconBtn->setPosition(currentPos);
        currentPos = iconBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* dupSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
        dupSpr->setScale(0.8f);
        CCMenuItemSpriteExtra* dupBtn = CCMenuItemSpriteExtra::create(dupSpr, nullptr, this,
            menu_selector(LevelListLayer::confirmClone));
        m_buttonMenu->addChild(dupBtn);
        dupBtn->setPosition(currentPos);
        currentPos = dupBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* shareSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_shareBtn_001.png");
        shareSpr->setScale(refreshSpr->getContentSize().width * refreshSpr->getScale() / shareSpr->getContentSize().width);
        CCMenuItemSpriteExtra* shareBtn = CCMenuItemSpriteExtra::create(shareSpr, nullptr, this,
            menu_selector(LevelListLayer::onShare));
        m_buttonMenu->addChild(shareBtn);
        shareBtn->setPosition(currentPos);
    } else {
        std::string titleStr = m_levelList->m_listName;
        cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(titleStr.c_str(), "bigFont.fnt");
        addChild(titleLabel, 20);
        titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, director->getScreenTop() - 20.0f));
        titleLabel->limitLabelWidth(240.0f, 0.8f, 0.0f);

        std::string creatorStr = "By " + m_levelList->m_creatorName;
        cocos2d::CCLabelBMFont* creatorLabel = cocos2d::CCLabelBMFont::create(creatorStr.c_str(), "goldFont.fnt");
        creatorLabel->limitLabelWidth(90.0f, 0.8f, 0.0f);

        cocos2d::CCPoint creatorWorld = titleLabel->getPosition() + cocos2d::CCPoint(
            (creatorLabel->getContentSize().width * 0.5f * creatorLabel->getScale()) +
            (titleLabel->getContentSize().width * 0.5f * titleLabel->getScale()) + 10.0f,
            0.0f);

        CCMenuItemSpriteExtra* profileBtn = CCMenuItemSpriteExtra::create(creatorLabel, nullptr, this,
            menu_selector(LevelListLayer::onViewProfile));
        m_buttonMenu->addChild(profileBtn);
        profileBtn->setPosition(m_buttonMenu->convertToNodeSpace(creatorWorld));

        m_diffSprite->setScale(0.8f);
        m_featureSprite->setScale(0.8f);

        float diffWidth = m_diffSprite->getContentSize().width * m_diffSprite->getScale();
        float titleWidth = titleLabel->getContentSize().width * titleLabel->getScale();
        float creatorWidth = creatorLabel->getContentSize().width * creatorLabel->getScale();
        float labelOffset = -(diffWidth + titleWidth + creatorWidth + 20.0f) * 0.5f;

        titleLabel->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(labelOffset, 0.0f));
        profileBtn->setPosition(profileBtn->getPosition() + cocos2d::CCPoint(labelOffset, 0.0f));

        addChild(m_diffSprite, 2);
        addChild(m_featureSprite, 1);

        cocos2d::CCPoint diffPos = titleLabel->getPosition() + cocos2d::CCPoint(
            -((diffWidth * 0.5f) + (titleWidth * 0.5f)) - 10.0f, -1.0f);
        m_diffSprite->setPosition(diffPos);
        m_featureSprite->setPosition(m_diffSprite->getPosition() + cocos2d::CCPoint(0.0f, m_diffSprite->getScale() * -6.0f));

        m_unk2 = true;

        cocos2d::CCPoint btnsStart(director->getScreenRight() - 25.0f, director->getScreenTop() - 25.0f);
        cocos2d::CCPoint menuPos = m_buttonMenu->convertToNodeSpace(btnsStart);

        cocos2d::CCSprite* refreshSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        refreshSpr->setScale(0.8f);
        m_refreshBtn = CCMenuItemSpriteExtra::create(refreshSpr, nullptr, this,
            menu_selector(LevelListLayer::onRefreshLevelList));
        m_buttonMenu->addChild(m_refreshBtn);
        m_refreshBtn->setPosition(m_buttonMenu->convertToNodeSpace(btnsStart));
        cocos2d::CCPoint currentPos = m_refreshBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* infoSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
        infoSpr->setScale(0.8f);
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSpr, nullptr, this,
            menu_selector(LevelListLayer::onInfo));
        m_buttonMenu->addChild(infoBtn);
        infoBtn->setPosition(currentPos);
        currentPos = infoBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* likeSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_like2Btn_001.png");
        likeSpr->setScale(0.8f);
        m_likeButton = CCMenuItemSpriteExtra::create(likeSpr, nullptr, this,
            menu_selector(LevelListLayer::onLike));
        m_buttonMenu->addChild(m_likeButton);
        m_likeButton->setPosition(currentPos);
        currentPos = m_likeButton->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        cocos2d::CCSprite* dupSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
        dupSpr->setScale(0.8f);
        CCMenuItemSpriteExtra* dupBtn = CCMenuItemSpriteExtra::create(dupSpr, nullptr, this,
            menu_selector(LevelListLayer::confirmClone));
        m_buttonMenu->addChild(dupBtn);
        dupBtn->setPosition(currentPos);
        currentPos = dupBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        std::string heartFrame = m_levelList->m_favorite ? "gj_heartOn_001.png" : "gj_heartOff_001.png";
        cocos2d::CCSprite* heartSpr = cocos2d::CCSprite::createWithSpriteFrameName(heartFrame.c_str());
        CCMenuItemSpriteExtra* favBtn = CCMenuItemSpriteExtra::create(heartSpr, nullptr, this,
            menu_selector(LevelListLayer::onFavorite));
        favBtn->setSizeMult(1.1f);
        m_buttonMenu->addChild(favBtn, 100);
        favBtn->setPosition(currentPos);
        currentPos = favBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);

        GJAccountManager* AM = GJAccountManager::sharedState();
        if (m_levelList->m_accountID == AM->m_accountID - AM->m_unkInt1) {
            cocos2d::CCSprite* serverDelSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteServerBtn_001.png");
            serverDelSpr->setScale(0.8f);
            CCMenuItemSpriteExtra* serverDelBtn = CCMenuItemSpriteExtra::create(serverDelSpr, nullptr, this,
                menu_selector(LevelListLayer::confirmOwnerDelete));
            m_buttonMenu->addChild(serverDelBtn);
            serverDelBtn->setPosition(currentPos);
            currentPos = serverDelBtn->getPosition() + cocos2d::CCPoint(0.0f, -41.0f);
        }
    }

    cocos2d::CCSprite* infoIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoIcon, nullptr, this,
        menu_selector(LevelListLayer::onListInfo));
    infoBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(infoBtn);
    cocos2d::CCPoint infoLocal(30.0f, 30.0f);
    infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(infoLocal));

    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = this;
    m_circle->m_fade = false;
    m_circle->retain();
    m_circle->show();
    m_circle->setVisible(false);

    m_noInternet = TextArea::create(
        "No <cg>Internet</c> connection!",
        "bigFont.fnt",
        1.0f,
        600.0f,
        cocos2d::CCPoint(0.5f, 1.0f),
        20.0f,
        false
    );
    m_noInternet->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f));
    m_noInternet->setScale(0.6f);
    addChild(m_noInternet, 2);
    m_noInternet->setVisible(false);

    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, closeButton);
    }

    updateSideButtons();
    updateStatsArt();

    if (isEditor || m_levelList->m_onlineLevelsLoaded) {
        m_cached = true;
        setupLevelBrowser(nullptr);
    } else {
        searchObject->m_searchMode = 1;
        m_levelList->m_onlineLevelsLoaded = true;
        loadPage(searchObject);
    }

    return true;
}

void LevelListLayer::levelListDeleteFailed(int id) {
    if (m_levelList->m_listID == id) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Error", "List delete failed, please try again later.");
        alert->show();
        m_circle->setVisible(false);
    }
}

void LevelListLayer::levelListDeleteFinished(int id) {
    if (m_levelList->m_listID == id) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "List Deleted", "The list has been removed from the server.");
        alert->show();
        m_circle->setVisible(false);
    }
}

void LevelListLayer::likedItem(LikeItemType type, int id, bool liked) {
    if (m_levelList->m_listID == id) {
        updateSideButtons();
        m_levelList->m_likes += liked ? 1 : -1;
        updateStatsArt();
    }
}

void LevelListLayer::loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int type) {
    if (m_searchObject && m_searchObject->m_searchType == SearchType::LevelListsOnClick) {
        int listID = atoi(m_searchObject->m_searchQuery.c_str());
        GameLevelManager::sharedState()->markListAsDownloaded(listID);
        if (listID == m_levelList->m_listID) {
            m_levelList->m_onlineLevelsLoaded = true;
        }
    }

    if (type != 1) {
        LevelBrowserLayer::loadLevelsFinished(levels, key, type);
        return;
    }

    if (m_searchKey2 == key) {
        updateStatsArt();
        const char* searchKey = m_searchObject->getKey();
        GameLevelManager::sharedState()->resetTimerForKey(searchKey);
        loadPage(m_searchObject);
    }
}

void LevelListLayer::onBack(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    setKeypadEnabled(false);

    if (m_titleInput) {
        m_titleInput->onClickTrackNode(false);
    }
    verifyListName();

    GameLevelManager::sharedState()->m_searchKey = m_searchKey1;

    if (m_levelList->m_listType == GJLevelType::Editor) {
        cocos2d::CCScene* searchScene = GameLevelManager::sharedState()->getSearchScene(
            GameLevelManager::sharedState()->m_searchKey.c_str());

        if (GameLevelManager::sharedState()->m_returnToLocalLevels || !searchScene) {
            GameLevelManager::sharedState()->m_returnToLocalLevels = false;
            GJSearchObject* searchObj = GJSearchObject::create(SearchType::MyLists);
            searchScene = LevelBrowserLayer::scene(searchObj);
        }

        cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, searchScene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
    } else {
        LevelBrowserLayer::onBack(sender);
    }
}

void LevelListLayer::onClaimReward(cocos2d::CCObject* sender) {
    int completed = GJLevelList::completedLevels(m_levelList);
    int remaining = m_levelList->m_levelsToClaim - completed;

    if (remaining <= 0) {
        if (GameStatsManager::sharedState()->claimListReward(m_levelList)) {
            FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");

            int diamonds = m_levelList->m_diamonds;
            CurrencyRewardLayer* reward = CurrencyRewardLayer::create(
                0, 0, 0, diamonds,
                CurrencySpriteType::Icon, 0,
                CurrencySpriteType::Icon, 0,
                cocos2d::CCPoint(0.0f, 1.0f),
                m_rewardPosition,
                0.0f, 0.0f
            );
            addChild(reward, 14);
        }
        static_cast<CCMenuItemSpriteExtra*>(sender)->setEnabled(false);
        sender->removeFromParentAndCleanup(true);
    } else {
        const char* levelWord = (remaining == 1) ? "level" : "levels";
        std::string message = cocos2d::CCString::createWithFormat(
            "Complete <co>%i</c> more <cg>%s</c> from this <cl>list</c> to claim the <cy>reward</c>!",
            remaining, levelWord
        )->getCString();

        DialogObject* dialogObj = DialogObject::create("The Mechanic", message, 37, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        DialogLayer* dialog = DialogLayer::create(dialogObj, true);
        dialog->addToMainScene();
        dialog->updateChatPlacement(DialogChatPlacement::Center);
        dialog->animateIn(DialogAnimationType::FromCenter);
    }
}

void LevelListLayer::onDelete() {
    m_buttonMenu->setEnabled(false);
    GameLevelManager::sharedState()->deleteLevelList(m_levelList);
    onBack(nullptr);
    m_exiting = true;
}

void LevelListLayer::onDescription(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    std::string desc = m_levelList->getUnpackedDescription();
    ShareCommentLayer* layer = ShareCommentLayer::create(
        "Description",
        300,
        CommentType::ListDescription,
        m_levelList->m_M_ID,
        desc
    );
    layer->m_delegate = this;
    layer->show();
}

void LevelListLayer::onEnter() {
    cocos2d::CCLayer::onEnter();
    if (m_exited) {
        updateStatsArt();
    }
}

void LevelListLayer::onExit() {
    cocos2d::CCLayer::onExit();
    m_exited = true;
}

void LevelListLayer::onFavorite(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    bool wasFavorite = m_levelList->m_favorite;
    m_levelList->m_favorite = !wasFavorite;

    cocos2d::CCNode* image = static_cast<CCMenuItemSpriteExtra*>(sender)->getNormalImage();
    const char* frame = wasFavorite ? "gj_heartOff_001.png" : "gj_heartOn_001.png";
    image->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame));

    if (!wasFavorite) {
        GameLevelManager::sharedState()->saveLevelList(m_levelList);
    } else {
        GameLevelManager::sharedState()->deleteLevelList(m_levelList);
    }
}

void LevelListLayer::onInfo(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    InfoLayer* info = InfoLayer::create(nullptr, nullptr, m_levelList);
    info->show();
}

void LevelListLayer::onLike(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    GameManager* GM = GameManager::sharedState();
    if (GM->m_playerUserID.random == GM->m_playerUserID.seed) {
        GameLevelManager::sharedState()->updateUserScore();
    }

    LikeItemLayer* layer = LikeItemLayer::create(LikeItemType::LevelList, m_levelList->m_listID, 0);
    layer->m_likeDelegate = this;
    layer->show();
}

void LevelListLayer::onListInfo(cocos2d::CCObject* sender) {
    m_levelList->showListInfo();
}

void LevelListLayer::onRefreshLevelList(cocos2d::CCObject* sender) {
    GameLevelManager::sharedState()->m_levelManagerDelegate = this;
    if (m_refreshBtn) {
        m_refreshBtn->setVisible(false);
    }
    m_circle->setVisible(true);
    setupLevelBrowser(nullptr);

    GJSearchObject* searchObj = GJSearchObject::create(SearchType::LevelListsOnClick);
    searchObj->m_searchQuery = GameToolbox::intToString(m_levelList->m_listID);
    searchObj->m_searchMode = 1;

    const char* key = searchObj->getKey();
    m_searchKey2 = key;
    GameLevelManager::sharedState()->resetTimerForKey(m_searchKey2.c_str());
    GameLevelManager::sharedState()->getLevelLists(searchObj);
}

void LevelListLayer::onSelectIcon(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    SelectListIconLayer* layer = SelectListIconLayer::create(m_levelList->m_difficulty);
    layer->m_delegate = this;
    layer->show();
}

void LevelListLayer::onShare(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    if (m_titleInput) {
        m_titleInput->onClickTrackNode(false);
    }

    GJAccountManager* AM = GJAccountManager::sharedState();
    if (AM->m_accountID - AM->m_unkInt1 <= 0) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Account Needed",
            "You need an <cl>account</c> to share <cg>lists</c>. You can create one for free from settings in the main menu.");
        alert->show();
    } else {
        ShareListLayer* layer = ShareListLayer::create(m_levelList);
        layer->show();
    }
}

void LevelListLayer::onToggleEditMode(cocos2d::CCObject* sender) {
    m_editMode++;
    if (m_editMode > 2) {
        m_editMode = 0;
    }
    updateEditMode();
}

void LevelListLayer::onViewProfile(cocos2d::CCObject* sender) {
    if (m_exiting) {
        return;
    }

    ProfilePage* page = ProfilePage::create(m_levelList->m_accountID, false);
    page->show();
}

void LevelListLayer::ownerDelete() {
    if (m_exiting) {
        return;
    }

    GameLevelManager::sharedState()->m_levelListDeleteDelegate = this;
    GameLevelManager::sharedState()->deleteServerLevelList(m_levelList->m_listID);
    m_circle->setVisible(true);
}

cocos2d::CCScene* LevelListLayer::scene(GJLevelList* list) {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    AppDelegate::get()->m_scene = scene;
    LevelListLayer* layer = LevelListLayer::create(list);
    scene->addChild(layer);
    return scene;
}

void LevelListLayer::setIDPopupClosed(SetIDPopup* popup, int value) {
    if (popup->getTag() == 1003) {
        GJLevelList::reorderLevel(m_levelList, popup->m_levelID, value - 1);
        setupLevelBrowser(nullptr);
        updateEditMode();
    } else {
        LevelBrowserLayer::setIDPopupClosed(popup, value);
    }
}

void LevelListLayer::shareCommentClosed(std::string text, ShareCommentLayer* layer) {
    m_levelList->m_listDesc = LevelTools::base64EncodeString(text);
}

void LevelListLayer::textChanged(CCTextInputNode* node) {
    if (node->getTag() == 1002) {
        m_levelList->m_listName = node->getString();
    }
}

void LevelListLayer::textInputClosed(CCTextInputNode* node) {
    if (node->getTag() == 1002) {
        m_levelList->m_listName = node->getString();
        node->updateLabel(node->getString());
    }
}

void LevelListLayer::textInputOpened(CCTextInputNode* node) {
    if (node->getTag() == 1002) {
        if (node->getString().empty()) {
            node->updateLabel(" ");
        }
    }
}

void LevelListLayer::updateEditMode() {
    if (m_list) {
        CustomListView* listView = static_cast<CustomListView*>(m_list->m_listView);
        listView->m_cellMode = m_editMode;
        listView->reloadAll();
    }
}

cocos2d::CCArray* LevelListLayer::updateResultArray(cocos2d::CCArray* results) {
    return GJLevelList::getListLevelsArray(m_levelList, results);
}

void LevelListLayer::updateSideButtons() {
    if (m_likeButton) {
        int dummy = 0;
        if (GameLevelManager::sharedState()->hasLikedItemFullCheck(
                LikeItemType::LevelList, m_levelList->m_listID, false, dummy)) {
            m_likeButton->setEnabled(false);
            m_likeButton->getNormalImage()->setDisplayFrame(
                cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_like2Btn2_001.png"));
        }
    }
}

void LevelListLayer::updateStatsArt() {
    m_exited = false;

    if (m_objects) {
        for (unsigned int i = 0; i < m_objects->count(); ++i) {
            cocos2d::CCNode* obj = static_cast<cocos2d::CCNode*>(m_objects->objectAtIndex(i));
            obj->removeFromParentAndCleanup(true);
        }
    }
    m_objects->removeAllObjects();

    std::string diffFrame = GJLevelList::frameForListDifficulty(m_levelList->m_difficulty, DifficultyIconType::NoText);
    m_diffSprite->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(diffFrame.c_str()));
    m_featureSprite->setVisible(m_levelList->m_featured);

    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = director->getWinSize();

    int completed = GJLevelList::completedLevels(m_levelList);
    int totalLevels = GJLevelList::totalLevels(m_levelList);
    int diamonds = m_levelList->m_diamonds;
    bool hasReward = false;
    bool claimed = false;

    if (diamonds > 0 && m_levelList->m_levelsToClaim > 0) {
        hasReward = true;
        claimed = GameStatsManager::sharedState()->hasClaimedListReward(m_levelList);
        if (!claimed) {
            totalLevels = m_levelList->m_levelsToClaim;
        }
    }

    cocos2d::CCSprite* progressBg = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    progressBg->setColor(cocos2d::ccc3(0, 0, 0));
    progressBg->setOpacity(125);
    progressBg->setScaleX(0.8f);
    progressBg->setScaleY(0.8f);
    addChild(progressBg, 3);
    m_objects->addObject(progressBg);
    progressBg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, 16.0f));

    cocos2d::CCSprite* progressBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    progressBar->setScaleX(0.98f);
    progressBar->setScaleY(0.76f);
    if (hasReward && !claimed) {
        progressBar->setColor(cocos2d::ccc3(255, 255, 50));
    } else {
        progressBar->setColor(cocos2d::ccc3(50, 255, 50));
    }
    progressBar->setOpacity(191);
    progressBg->addChild(progressBar, 1);
    float barOffset = progressBar->getContentSize().width - (progressBar->getContentSize().width * 0.98f);
    progressBar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    progressBar->setPosition(cocos2d::CCPoint(barOffset * 0.5f, progressBg->getContentSize().height * 0.5f));

    float fillPercent = (totalLevels <= 0) ? 0.0f : static_cast<float>(completed) / static_cast<float>(totalLevels);
    float fillWidth = fillPercent * progressBar->getContentSize().width;
    if (fillWidth > progressBar->getContentSize().width) {
        fillWidth = progressBar->getContentSize().width;
    }
    progressBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, fillWidth, progressBar->getContentSize().height));

    std::string progressStr = cocos2d::CCString::createWithFormat("%i/%i", completed, totalLevels)->getCString();
    cocos2d::CCLabelBMFont* progressLabel = cocos2d::CCLabelBMFont::create(progressStr.c_str(), "bigFont.fnt");
    addChild(progressLabel, 4);
    m_objects->addObject(progressLabel);
    progressLabel->setPosition(progressBg->getPosition() + cocos2d::CCPoint(0.0f, 1.0f));
    progressLabel->setScale(0.5f);

    if (m_levelList->m_listType != GJLevelType::Editor) {
        float likesOffset = hasReward && !claimed ? 45.0f : 52.0f;

        cocos2d::CCSprite* downloadIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
        downloadIcon->setScale(0.8f);
        downloadIcon->setPosition(cocos2d::CCPoint(26.0f, winSize.height * 0.5f + 88.0f));
        addChild(downloadIcon, 1);
        m_objects->addObject(downloadIcon);

        std::string dlStr = GameToolbox::intToShortString(m_levelList->m_downloads);
        cocos2d::CCLabelBMFont* downloadLabel = cocos2d::CCLabelBMFont::create(dlStr.c_str(), "bigFont.fnt");
        downloadLabel->setScale(0.4f);
        downloadLabel->setPosition(downloadIcon->getPosition() + cocos2d::CCPoint(0.0f, -21.0f));
        addChild(downloadLabel, 1);
        m_objects->addObject(downloadLabel);

        const char* likeFrame = (m_levelList->m_likes < 0) ? "GJ_dislikesIcon_001.png" : "GJ_likesIcon_001.png";
        cocos2d::CCSprite* likeIcon = cocos2d::CCSprite::createWithSpriteFrameName(likeFrame);
        likeIcon->setScale(0.8f);
        float likeOffsetY = (m_levelList->m_likes < 0) ? 4.0f : 0.0f;
        likeIcon->setPosition(downloadIcon->getPosition() + cocos2d::CCPoint(0.0f, likeOffsetY - likesOffset));
        addChild(likeIcon, 1);
        m_objects->addObject(likeIcon);

        std::string likeStr = GameToolbox::intToShortString(m_levelList->m_likes);
        cocos2d::CCLabelBMFont* likeLabel = cocos2d::CCLabelBMFont::create(likeStr.c_str(), "bigFont.fnt");
        likeLabel->setScale(0.4f);
        likeLabel->setPosition(likeIcon->getPosition() + cocos2d::CCPoint(0.0f, -21.0f));
        addChild(likeLabel, 1);
        m_objects->addObject(likeLabel);

        if (hasReward) {
            if (claimed) {
                cocos2d::CCSprite* diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_diamondsIcon_001.png");
                addChild(diamondIcon);
                m_objects->addObject(diamondIcon);
                diamondIcon->setPosition(likeIcon->getPosition() + cocos2d::CCPoint(0.0f, -likesOffset));
                diamondIcon->setScale(0.9f);

                std::string diamondStr = GameToolbox::intToShortString(m_levelList->m_diamonds);
                cocos2d::CCLabelBMFont* diamondLabel = cocos2d::CCLabelBMFont::create(diamondStr.c_str(), "bigFont.fnt");
                diamondLabel->setScale(0.4f);
                diamondLabel->setPosition(diamondIcon->getPosition() + cocos2d::CCPoint(0.0f, -21.0f));
                addChild(diamondLabel, 1);
                m_objects->addObject(diamondLabel);
            } else {
                cocos2d::CCSprite* diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
                addChild(diamondIcon, 10);
                m_objects->addObject(diamondIcon);
                diamondIcon->setPosition(progressBg->getPosition() + cocos2d::CCPoint(125.0f, 0.0f));

                cocos2d::CCSprite* completesIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
                addChild(completesIcon);
                m_objects->addObject(completesIcon);
                completesIcon->setPosition(likeIcon->getPosition() + cocos2d::CCPoint(0.0f, -likesOffset));
                completesIcon->setScale(0.8f);

                int total = GJLevelList::totalLevels(m_levelList);
                std::string completesStr = cocos2d::CCString::createWithFormat("%i/%i", completed, total)->getCString();
                cocos2d::CCLabelBMFont* completesLabel = cocos2d::CCLabelBMFont::create(completesStr.c_str(), "bigFont.fnt");
                addChild(completesLabel);
                m_objects->addObject(completesLabel);
                completesLabel->setPosition(completesIcon->getPosition() + cocos2d::CCPoint(0.0f, -21.0f));
                completesLabel->setScale(0.4f);

                cocos2d::CCNode* rewardNode = cocos2d::CCNode::create();
                rewardNode->setContentSize(cocos2d::CCSize(50.0f, 50.0f));

                cocos2d::CCNode* innerNode = cocos2d::CCNode::create();
                innerNode->setContentSize(cocos2d::CCSize(50.0f, 50.0f));
                rewardNode->addChild(innerNode);
                innerNode->setPosition(cocos2d::CCPoint(
                    rewardNode->getContentSize().width * 0.5f,
                    rewardNode->getContentSize().height * 0.5f));

                cocos2d::CCSprite* rewardSprite = nullptr;
                if (completed >= m_levelList->m_levelsToClaim) {
                    rewardSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png");
                } else {
                    CCSpriteGrayscale* gray = CCSpriteGrayscale::createWithSpriteFrameName("GJ_rewardBtn_001.png");
                    gray->setEvenLuminance(1.0f);
                    gray->setColor(cocos2d::ccc3(175, 175, 175));
                    rewardSprite = gray;
                }
                rewardSprite->setScale(0.6f);
                innerNode->addChild(rewardSprite);

                CCMenuItemSpriteExtra* rewardBtn = CCMenuItemSpriteExtra::create(rewardNode, nullptr, this,
                    menu_selector(LevelListLayer::onClaimReward));
                m_buttonMenu->addChild(rewardBtn);

                m_rewardPosition = completesIcon->getPosition() + cocos2d::CCPoint(0.0f, -54.0f);
                cocos2d::CCPoint localPos = m_buttonMenu->convertToNodeSpace(m_rewardPosition);
                rewardBtn->setPosition(localPos);

                cocos2d::CCSprite* rewardDiamond = cocos2d::CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
                innerNode->addChild(rewardDiamond);
                rewardDiamond->setPosition(rewardSprite->getPosition() + cocos2d::CCPoint(8.0f, -25.0f));

                std::string diamondStr = GameToolbox::intToString(m_levelList->m_diamonds);
                cocos2d::CCLabelBMFont* diamondLabel = cocos2d::CCLabelBMFont::create(diamondStr.c_str(), "bigFont.fnt");
                innerNode->addChild(diamondLabel);
                diamondLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
                diamondLabel->setPosition(rewardDiamond->getPosition() + cocos2d::CCPoint(-8.0f, 0.0f));
                diamondLabel->setScale(0.4f);

                m_objects->addObject(rewardBtn);
            }
        }
    }
}

void LevelListLayer::verifyListName() {
    if (m_levelList->m_listName.empty()) {
        if (!m_levelList->m_unkString.empty()) {
            m_levelList->m_listName = m_levelList->m_unkString;
        } else {
            m_levelList->m_listName = "Unknown";
        }
    }
}

LevelListLayer::~LevelListLayer() {
}