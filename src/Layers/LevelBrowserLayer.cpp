#include "Layers/LevelBrowserLayer.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Managers/GJAccountManager.h"
#include "Layers/EditLevelLayer.h"
#include "Layers/LevelListLayer.h"
#include "Popups/Setup/SetupSmartTemplateLayer.h"
#include "Popups/SetFolderPopup.h"
#include "Models/GJGameLevel.h"
#include "Models/GJLevelList.h"
#include "Editor/GJSmartTemplate.h"
#include "Managers/MusicDownloadManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "Managers/LocalLevelManager.h"
#include "UI/CustomListView.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCSpriteGrayscale.h"
#include "Cocos/CCURLObject.h"
#include "Layers/URLViewLayer.h"
#include "CCArray.h"
#include "CCString.h"
#include "CCDirector.h"
#include "CCTransitionFade.h"
#include "CCApplication.h"

LevelBrowserLayer* LevelBrowserLayer::create(GJSearchObject* object) {
    auto layer = new LevelBrowserLayer();
    if (layer && layer->init(object)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool LevelBrowserLayer::init(GJSearchObject* object) {
    cocos2d::ccColor4B color = cocos2d::ccc4(0, 0, 0, 0);

    if (object && object->m_searchIsOverlay) {
        m_isOverlay = true;
        auto director = cocos2d::CCDirector::sharedDirector();
        auto touchDispatcher = director->getTouchDispatcher();
        touchDispatcher->registerForcePrio(this, 2);
        color = cocos2d::ccc4(0, 0, 0, 150);
    }

    if (!cocos2d::CCLayerColor::initWithColor(color)) {
        return false;
    }

    setSearchObject(object);

    if (!m_isOverlay) {
        if (object) {
            GameLevelManager::sharedState()->m_searchType = object->m_searchType;
        }
    } else {
        setTouchEnabled(true);
    }

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    if (!m_isOverlay) {
        auto bgSprite = cocos2d::CCSprite::create("GJ_gradientBG.png");
        bgSprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
        addChild(bgSprite, -2);
        bgSprite->setScaleX((winSize.width + 10.0f) / bgSprite->getContentSize().height);
        bgSprite->setScaleY((winSize.height + 10.0f) / bgSprite->getContentSize().width);
        bgSprite->setPosition(-5.0f, -5.0f);
        bgSprite->setColor(cocos2d::ccc3(102, 102, 255));

        auto sideArtLeft = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
        sideArtLeft->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
        sideArtLeft->setPosition(
            cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
            cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
        );
        addChild(sideArtLeft, 1);

        auto sideArtRight = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
        sideArtRight->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
        sideArtRight->setPosition(
            cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f,
            cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
        );
        sideArtRight->setFlipX(true);
        addChild(sideArtRight, 1);
    }

    const char* closeBtnName = m_isOverlay ? "GJ_closeBtn_001.png" : "GJ_arrow_01_001.png";
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName(closeBtnName);
    if (m_isOverlay) {
        closeSprite->setScale(0.9f);
    }

    auto closeButton = CCMenuItemSpriteExtra::create(
        closeSprite,
        nullptr,
        this,
        menu_selector(LevelBrowserLayer::onBack)
    );
    closeButton->setSizeMult(1.6f);

    auto closeMenu = cocos2d::CCMenu::create(closeButton, nullptr);
    addChild(closeMenu, 1);
    closeMenu->setPosition(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f
    );

    bool isControllerConnected = PlatformToolbox::isControllerConnected();

    auto mainMenu = cocos2d::CCMenu::create();
    addChild(mainMenu, 10);

    float centerY = winSize.height * 0.5f;

    const char* leftArrowName = isControllerConnected ? "controllerBtn_DPad_Left_001.png" : "GJ_arrow_03_001.png";
    auto leftSprite = cocos2d::CCSprite::createWithSpriteFrameName(leftArrowName);
    auto leftButton = CCMenuItemSpriteExtra::create(
        leftSprite,
        nullptr,
        this,
        menu_selector(LevelBrowserLayer::onPrevPage)
    );
    m_leftArrow = leftButton;
    leftButton->setSizeMult(2.0f);
    mainMenu->addChild(leftButton);

    auto leftPos = cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        centerY
    );
    leftButton->setPosition(mainMenu->convertToNodeSpace(leftPos));

    const char* rightArrowName = isControllerConnected ? "controllerBtn_DPad_Right_001.png" : "GJ_arrow_03_001.png";
    auto rightSprite = cocos2d::CCSprite::createWithSpriteFrameName(rightArrowName);
    if (!isControllerConnected) {
        rightSprite->setFlipX(true);
    }
    auto rightButton = CCMenuItemSpriteExtra::create(
        rightSprite,
        nullptr,
        this,
        menu_selector(LevelBrowserLayer::onNextPage)
    );
    m_rightArrow = rightButton;
    rightButton->setSizeMult(2.0f);
    mainMenu->addChild(rightButton);

    auto rightPos = cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f,
        centerY
    );
    rightButton->setPosition(mainMenu->convertToNodeSpace(rightPos));

    auto pageBg = cocos2d::CCSprite::create("GJ_button_02.png");
    auto pageLabel = cocos2d::CCLabelBMFont::create();
    m_pageText = pageLabel;
    pageBg->addChild(pageLabel, 1);
    pageLabel->setPosition(
        pageBg->getContentSize().width * 0.5f,
        pageBg->getContentSize().height * 0.5f + 1.0f
    );
    updatePageLabel();

    pageBg->setScale(0.7f);
    auto pageButton = CCMenuItemSpriteExtra::create(
        pageBg,
        nullptr,
        this,
        menu_selector(LevelBrowserLayer::onGoToPage)
    );
    m_pageBtn = pageButton;
    pageButton->setSizeMult(1.2f);
    mainMenu->addChild(pageButton);

    auto pageBtnPos = cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f + 1.0f,
        centerY + 122.0f
    );
    pageButton->setPosition(mainMenu->convertToNodeSpace(pageBtnPos));

    m_leftArrow->setVisible(false);
    m_rightArrow->setVisible(false);
    m_pageBtn->setVisible(false);

    auto loadingCircle = LoadingCircle::create();
    m_circle = loadingCircle;
    loadingCircle->m_parentLayer = this;
    loadingCircle->m_fade = 0;
    loadingCircle->retain();
    loadingCircle->show();
    loadingCircle->setVisible(false);

    auto countLabel = cocos2d::CCLabelBMFont::create();
    m_countText = countLabel;
    addChild(countLabel);
    countLabel->setPosition(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 7.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 3.0f
    );
    countLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
    countLabel->setScale(0.6f);

    auto noInternetText = TextArea::create(
        "No <cg>Internet</c> connection!",
        "bigFont.fnt",
        1.0f,
        600.0f,
        cocos2d::CCPoint(20.0f, 0.0f),
        0.0f,
        false
    );
    m_noInternet = noInternetText;
    noInternetText->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f);
    noInternetText->setScale(0.6f);
    addChild(noInternetText, 2);
    noInternetText->setVisible(false);

    if (m_searchObject) {
        auto searchType = m_searchObject->m_searchType;

        // Check for safe/weekly/event labels
        if (searchType == Safe || searchType == WeeklySafe || searchType == EventSafe) {
            auto label = cocos2d::CCSprite::createWithSpriteFrameName("theSafeLabel_001.png");
            addChild(label, 2);
            label->setPosition(
                winSize.width * 0.5f + 13.0f,
                winSize.height * 0.5f + 110.0f + 24.0f
            );
        } else if (searchType == FeaturedGDW) {
            auto label = cocos2d::CCSprite::createWithSpriteFrameName("featuredLabel_001.png");
            addChild(label, 2);
            label->setPosition(
                winSize.width * 0.5f,
                winSize.height * 0.5f + 110.0f + 24.0f
            );
        } else if (searchType == FeaturedLite) {
            auto label = cocos2d::CCSprite::createWithSpriteFrameName("weeklyLevelsLabel_001.png");
            addChild(label, 2);
            label->setPosition(
                winSize.width * 0.5f,
                winSize.height * 0.5f + 110.0f + 24.0f
            );
        } else if (searchType == BonusLevels) {
            auto label = cocos2d::CCSprite::createWithSpriteFrameName("bonusLevelsLabel_001.png");
            addChild(label, 2);
            label->setPosition(
                winSize.width * 0.5f,
                winSize.height * 0.5f + 110.0f + 24.0f
            );
        } else if (searchType == RecentMostLiked && m_searchObject->m_searchMode == 1) {
            auto label = cocos2d::CCSprite::createWithSpriteFrameName("topListsLabel_001.png");
            addChild(label, 2);
            label->setPosition(
                winSize.width * 0.5f,
                winSize.height * 0.5f + 110.0f + 24.0f
            );
        }

        // Delete/remove buttons for saved/favorite sections
        if ((searchType == SavedLevels || searchType == FavouriteLevels || searchType == MyLists || searchType == FavouriteLists) && !m_searchObject->m_searchIsOverlay) {
            auto deleteMenu = cocos2d::CCMenu::create();
            addChild(deleteMenu, 2);

            auto deleteSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_delBtn_001.png");
            auto deleteBtnSprite = ButtonSprite::create(deleteSprite, nullptr, 0, 1, 36.0f, 1.0f, "GJ_button_04.png", 0, false);
            deleteBtnSprite->setScale(0.4f);

            auto deleteButton = CCMenuItemSpriteExtra::create(
                deleteBtnSprite,
                nullptr,
                this,
                menu_selector(LevelBrowserLayer::onDeleteSelected)
            );
            deleteMenu->addChild(deleteButton);

            auto deletePos = cocos2d::CCPoint(
                winSize.width * 0.5f - 145.0f,
                winSize.height * 0.5f - 122.0f
            );
            deleteButton->setPosition(deleteMenu->convertToNodeSpace(deletePos));

            auto checkOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
            auto checkOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
            checkOn->setScale(0.5f);
            checkOff->setScale(0.5f);

            auto toggler = CCMenuItemToggler::create(
                checkOff,
                checkOn,
                this,
                menu_selector(LevelBrowserLayer::onToggleAllObjects)
            );
            m_allObjectsToggler = toggler;
            deleteMenu->addChild(toggler);

            auto togglerPos = deleteButton->getPosition() + cocos2d::CCPoint(26.0f, 0.0f);
            toggler->setPosition(togglerPos);
            toggler->setSizeMult(1.1f);

            auto togglerLabel = cocos2d::CCLabelBMFont::create();
            addChild(togglerLabel, 10);
            togglerLabel->setPosition(deletePos + cocos2d::CCPoint(55.0f, 0.0f));
            togglerLabel->setScale(0.4f);
        }

        // New/My Online Levels buttons
        if (searchType == MyLevels || searchType == MyLists || searchType == SavedLevels || searchType == FavouriteLevels) {
            auto newMenu = cocos2d::CCMenu::create();
            addChild(newMenu, 2);

            auto newSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png");
            auto newButton = CCMenuItemSpriteExtra::create(
                newSprite,
                nullptr,
                this,
                menu_selector(LevelBrowserLayer::onNew)
            );
            newMenu->addChild(newButton);
            newMenu->setPosition(winSize.width * 0.5f, (winSize.height - 220.0f) * 0.5f - 4.0f);

            float buttonSize = newSprite->getContentSize().height * 0.5f + 4.0f;
            newMenu->setPosition(
                cocos2d::CCDirector::sharedDirector()->getScreenRight() - buttonSize,
                buttonSize + cocos2d::CCDirector::sharedDirector()->getScreenBottom()
            );

            if (!m_isOverlay) {
                auto gameManager = GameManager::sharedState();
                auto accManager = GJAccountManager::sharedState();

                if (gameManager->m_playerUserID.random != gameManager->m_playerUserID.seed ||
                    accManager->m_accountID != accManager->m_unkInt1) {
                    auto onlineSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_myServerBtn_001.png");
                    auto onlineButton = CCMenuItemSpriteExtra::create(
                        onlineSprite,
                        nullptr,
                        this,
                        menu_selector(LevelBrowserLayer::onMyOnlineLevels)
                    );
                    newMenu->addChild(onlineButton);

                    auto onlinePos = cocos2d::CCPoint(
                        buttonSize + cocos2d::CCDirector::sharedDirector()->getScreenLeft(),
                        buttonSize + cocos2d::CCDirector::sharedDirector()->getScreenBottom()
                    );
                    onlineButton->setPosition(newMenu->convertToNodeSpace(onlinePos));
                }

                const char* modeBtnName = (searchType == MyLevels) ? "GJ_viewListsBtn_001.png" : "GJ_viewLevelsBtn_001.png";
                auto modeSprite = cocos2d::CCSprite::createWithSpriteFrameName(modeBtnName);
                modeSprite->setScale(0.8f);

                auto modeButton = CCMenuItemSpriteExtra::create(
                    modeSprite,
                    nullptr,
                    this,
                    menu_selector(LevelBrowserLayer::onLocalMode)
                );
                newMenu->addChild(modeButton);

                auto modePos = newButton->getPosition() + cocos2d::CCPoint(2.0f, 52.0f);
                modeButton->setPosition(modePos);
            }
        }

        // Refresh button
        if (searchType == Recent || searchType == MostLiked || searchType == HallOfFame ||
            searchType == RecentMostLiked || searchType == FeaturedGDW || searchType == DailySafe ||
            searchType == WeeklySafe || searchType == EventSafe || searchType == LevelListsOnClick ||
            searchType == Type26 || searchType == FeaturedLite || searchType == BonusLevels) {
            auto refreshMenu = cocos2d::CCMenu::create();
            addChild(refreshMenu, 2);

            auto refreshSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
            auto refreshButton = CCMenuItemSpriteExtra::create(
                refreshSprite,
                nullptr,
                this,
                menu_selector(LevelBrowserLayer::onRefresh)
            );
            m_refreshBtn = refreshButton;
            refreshMenu->addChild(refreshButton);
            refreshButton->setVisible(false);

            float btnSize = refreshSprite->getContentSize().width * 0.5f + 4.0f;
            refreshMenu->setPosition(
                cocos2d::CCDirector::sharedDirector()->getScreenRight() - btnSize,
                btnSize + cocos2d::CCDirector::sharedDirector()->getScreenBottom()
            );
        }

        // Delete/Remove all buttons for saved/favorite sections
        if (searchType == SavedLevels || searchType == FavouriteLevels || searchType == FavouriteLists) {
            auto deleteAllMenu = cocos2d::CCMenu::create();
            addChild(deleteAllMenu, 2);

            auto deleteAllSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
            auto deleteAllButton = CCMenuItemSpriteExtra::create(
                deleteAllSprite,
                nullptr,
                this,
                (searchType == FavouriteLevels || searchType == FavouriteLists) ?
                    menu_selector(LevelBrowserLayer::onRemoveAllFavorites) :
                    menu_selector(LevelBrowserLayer::onDeleteAll)
            );
            deleteAllMenu->addChild(deleteAllButton);

            float btnSize = deleteAllSprite->getContentSize().width * 0.5f + 4.0f;
            deleteAllMenu->setPosition(
                cocos2d::CCDirector::sharedDirector()->getScreenRight() - btnSize,
                btnSize + cocos2d::CCDirector::sharedDirector()->getScreenBottom()
            );

            const char* modeBtnName = (searchType == FavouriteLists) ? "GJ_viewLevelsBtn_001.png" : "GJ_viewListsBtn_001.png";
            auto modeSprite = cocos2d::CCSprite::createWithSpriteFrameName(modeBtnName);
            modeSprite->setScale(0.8f);

            auto modeButton = CCMenuItemSpriteExtra::create(
                modeSprite,
                nullptr,
                this,
                menu_selector(LevelBrowserLayer::onSavedMode)
            );
            deleteAllMenu->addChild(modeButton);

            auto modePos = deleteAllButton->getPosition() + cocos2d::CCPoint(2.0f, 52.0f);
            modeButton->setPosition(modePos);

            if (searchType != FavouriteLists) {
                const char* heartBtnName = (searchType == FavouriteLevels) ? "gj_heartOff_001.png" : "gj_heartOn_001.png";
                auto heartSprite = cocos2d::CCSprite::createWithSpriteFrameName(heartBtnName);
                auto heartButton = CCMenuItemSpriteExtra::create(
                    heartSprite,
                    nullptr,
                    this,
                    (searchType == FavouriteLevels) ?
                        menu_selector(LevelBrowserLayer::onSaved) :
                        menu_selector(LevelBrowserLayer::onFavorites)
                );
                heartButton->setSizeMult(1.1f);
                deleteAllMenu->addChild(heartButton, 100);

                auto heartPos = cocos2d::CCPoint(
                    btnSize,
                    btnSize + cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 50.0f
                );
                heartButton->setPosition(deleteAllMenu->convertToNodeSpace(heartPos));
            }

            bool showExtraButtons = true;
        }

        // Info button
        auto infoMenu = cocos2d::CCMenu::create();
        addChild(infoMenu, 2);

        auto infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        auto infoButton = CCMenuItemSpriteExtra::create(
            infoSprite,
            nullptr,
            this,
            menu_selector(LevelBrowserLayer::onInfo)
        );
        infoButton->setSizeMult(2.0f);
        infoMenu->addChild(infoButton);

        infoMenu->setPosition(
            cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 30.0f,
            cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 30.0f
        );

        if (true) {
            if (GJSearchObject::isLevelSearchObject(m_searchObject)) {
                auto folderSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
                auto folderLabel = cocos2d::CCLabelBMFont::create();
                m_folderText = folderLabel;
                folderSprite->addChild(folderLabel, 1);
                folderLabel->setPosition(
                    folderSprite->getContentSize().width * 0.5f,
                    folderSprite->getContentSize().height * 0.5f + 0.5f
                );

                auto folderButton = CCMenuItemSpriteExtra::create(
                    folderSprite,
                    nullptr,
                    this,
                    menu_selector(LevelBrowserLayer::onGoToFolder)
                );
                m_folderBtn = folderButton;
                folderButton->setSizeMult(1.2f);
                mainMenu->addChild(folderButton);

                auto folderPos = m_pageBtn->getPosition() + cocos2d::CCPoint(0.0f, -35.0f);
                folderButton->setPosition(folderPos);
            }

            auto lastSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            lastSprite->setScale(0.4f);
            auto lastSpriteInner = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            lastSprite->addChild(lastSpriteInner, 1);
            lastSpriteInner->setPosition(15.0f, 0.0f);
            lastSpriteInner->setFlipX(true);
            lastSprite->setFlipX(true);

            auto lastButton = CCMenuItemSpriteExtra::create(
                lastSprite,
                nullptr,
                this,
                menu_selector(LevelBrowserLayer::onGoToLastPage)
            );
            m_lastBtn = lastButton;
            lastButton->setSizeMult(1.2f);
            mainMenu->addChild(lastButton);

            auto lastPos = m_pageBtn->getPosition() + cocos2d::CCPoint(-2.0f, -65.0f);
            lastButton->setPosition(lastPos);

            auto searchSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
            auto searchButton = CCMenuItemSpriteExtra::create(
                searchSprite,
                nullptr,
                this,
                menu_selector(LevelBrowserLayer::onSearch)
            );
            searchButton->setSizeMult(1.2f);
            mainMenu->addChild(searchButton);

            auto searchPos = cocos2d::CCPoint(24.0f, centerY + 122.0f - 30.0f);
            searchButton->setPosition(mainMenu->convertToNodeSpace(searchPos));

            auto cancelSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_findBtnOff_001.png");
            auto cancelButton = CCMenuItemSpriteExtra::create(
                cancelSprite,
                nullptr,
                this,
                menu_selector(LevelBrowserLayer::onClearSearch)
            );
            m_cancelSearchBtn = cancelButton;
            cancelButton->setSizeMult(1.2f);
            mainMenu->addChild(cancelButton);

            auto cancelPos = searchButton->getPosition() + cocos2d::CCPoint(0.0f, -35.0f);
            cancelButton->setPosition(cancelPos);
            cancelButton->setVisible(false);
        }
    }

    if (isControllerConnected) {
        GameToolbox::addBackButton(this, closeButton);
        GameToolbox::addRThumbScrollButton(this);
    }

    loadPage(object);

    return true;
}

void LevelBrowserLayer::loadPage(GJSearchObject* object) {
    if (m_allObjectsToggler) {
        m_allSelected = false;
        m_allObjectsToggler->toggle(false);
    }

    setSearchObject(object);

    auto key = m_searchObject->getKey();
    m_circle->setVisible(true);

    if (m_refreshBtn) {
        m_refreshBtn->setVisible(false);
    }
    if (m_cancelSearchBtn) {
        m_cancelSearchBtn->setVisible(false);
    }
    if (!m_unk2) {
        m_leftArrow->setVisible(m_searchObject->m_page > 0);
    }

    auto searchType = m_searchObject->m_searchType;

    // Local levels/lists
    if (searchType == SavedLevels || searchType == FavouriteLevels || searchType == MyLevels ||
        searchType == MyLists || searchType == FavouriteLists || searchType == SmartTemplates) {

        cocos2d::CCArray* items = nullptr;

        if (searchType == MyLevels) {
            items = LocalLevelManager::sharedState()->getCreatedLevels(m_searchObject->m_folder);
        } else if (searchType == MyLists) {
            items = LocalLevelManager::sharedState()->getCreatedLists(m_searchObject->m_folder);
        } else if (searchType == FavouriteLevels) {
            items = GameLevelManager::sharedState()->getSavedLevels(true, object->m_folder);
        } else if (searchType == SmartTemplates) {
            items = GameLevelManager::sharedState()->getAllSmartTemplates();
        } else if (searchType == FavouriteLists) {
            items = GameLevelManager::sharedState()->getSavedLevelLists(object->m_folder);
        } else {
            items = GameLevelManager::sharedState()->getSavedLevels(false, object->m_folder);
        }

        // Filter by search query
        if (!m_searchObject->m_searchQuery.empty()) {
            items = getItemsMatchingSearch(items, m_searchObject->m_searchQuery, m_searchObject);
            if (m_cancelSearchBtn) {
                m_cancelSearchBtn->setVisible(true);
            }
        }

        auto pageItems = cocos2d::CCArray::create();

        auto gameManager = GameManager::sharedState();
        int itemsPerPage = gameManager->getGameVariable("0093") ? 20 : 10;

        int total = items->count();
        int startIdx = itemsPerPage * m_searchObject->m_page;
        m_lastPage = (int)ceilf((float)total / (float)itemsPerPage) - 1;

        for (int i = startIdx; i < startIdx + itemsPerPage && i < total; ++i) {
            pageItems->addObject(items->objectAtIndex(i));
        }

        if (!m_unk2) {
            m_rightArrow->setVisible((startIdx + pageItems->count()) < total);
        }

        auto pageInfo = GameLevelManager::sharedState()->createPageInfo(total, startIdx, itemsPerPage);
        setupPageInfo(pageInfo, key);

        loadLevelsFinished(pageItems, (const unsigned char*)key, searchType);

        if (m_itemCount > 0) {
            if (m_countText) {
                m_countText->setVisible(true);
            }
        } else {
            if (m_countText) {
                m_countText->setVisible(false);
            }

            auto director = cocos2d::CCDirector::sharedDirector();
            auto winSize = director->getWinSize();

            TextArea* emptyText = nullptr;
            std::string message;

            switch (searchType) {
                case SavedLevels:
                    message = "You have not <cg>downloaded</c> any <cl>levels</c> yet!";
                    break;
                case FavouriteLists:
                    message = "You have not <cg>favorited</c>\nany <cl>lists</c> yet!";
                    break;
                case FavouriteLevels:
                    message = "Levels marked as <cg>Favorite</c> will show up here!";
                    break;
                case MyLevels:
                    message = "Tap <cg>New</c> to create a <cl>level</c>!";
                    break;
                case MyLists:
                    message = "Tap <cg>New</c> to create a <cl>list</c>!";
                    break;
                case SmartTemplates:
                    message = "Tap <cg>New</c> to create a <cl>Template</c>!";
                    break;
                default:
                    break;
            }

            if (!message.empty()) {
                emptyText = TextArea::create(
                    " ",
                    "bigFont.fnt",
                    1.0f,
                    600.0f,
                    cocos2d::CCPoint(20.0f, 0.0f),
                    0.0f,
                    false
                );
                emptyText->m_height = 40.0f;
                emptyText->setString(message);
                emptyText->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f);
                emptyText->setScale(0.6f);
                addChild(emptyText);
            }
        }

    } else {
        // Online levels
        if (!m_unk2) {
            auto gameLevelManager = GameLevelManager::sharedState();
            auto storedLevels = gameLevelManager->getStoredOnlineLevels(m_searchObject->getNextPageKey());
            m_rightArrow->setVisible(storedLevels != nullptr);
        }

        auto gameLevelManager = GameLevelManager::sharedState();
        auto storedLevels = gameLevelManager->getStoredOnlineLevels(key);

        if (!storedLevels) {
            if (!m_cached) {
                setupLevelBrowser(nullptr);
            }

            GameLevelManager::sharedState()->m_levelManagerDelegate = this;

            if (searchType == Users) {
                GameLevelManager::sharedState()->getUsers(object);
            } else if (searchType == MapPacks) {
                GameLevelManager::sharedState()->getMapPacks(object);
            } else if (m_searchObject->m_searchMode == 1) {
                GameLevelManager::sharedState()->getLevelLists(object);
            } else {
                GameLevelManager::sharedState()->getOnlineLevels(object);
            }

            if (m_countText) {
                m_countText->setString(" ");
            }
        } else {
            auto pageInfo = GameLevelManager::sharedState()->getPageInfo(key);
            setupPageInfo(pageInfo, key);
            loadLevelsFinished(storedLevels, (const unsigned char*)key, searchType);
        }
    }

    if (!m_isOverlay) {
        auto gameLevelManager = GameLevelManager::sharedState();
        gameLevelManager->m_searchKey = key;
    }

    if (m_searchObject->m_searchType == MapPacks) {
        GameLevelManager::sharedState()->m_mapPackKey = key;
    }

    if (m_unk2) {
        // Cleanup
    } else {
        m_pageBtn->setVisible(m_rightArrow->isVisible() || m_leftArrow->isVisible());
    }

    // Cleanup key string
    if (key) {
        // String cleanup logic
    }
}

void LevelBrowserLayer::setupLevelBrowser(cocos2d::CCArray* items) {
    if (m_list) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto touchDispatcher = director->getTouchDispatcher();
        touchDispatcher->unregisterForcePrio(m_list);
        m_list->removeMeAndCleanup();
        m_list = nullptr;
    }

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float listHeight = (m_listHeight > 0) ? (float)m_listHeight : 220.0f;

    bool isTopList = (m_searchObject->m_searchMode == 1 && m_searchObject->m_searchType == RecentMostLiked) ||
                     (m_searchObject->m_searchType == Type26);

    if (isTopList && items) {
        items = updateResultArray(items);
    }

    CustomListView* listView = nullptr;
    if (items) {
        listView = CustomListView::create(items, BoomListType::Level, listHeight, 356.0f);
    }

    auto title = getSearchTitle();
    auto color = cocos2d::ccc4(67, 72, 133, 255);

    bool isType25or26 = (m_searchObject->m_searchType == Type25 || m_searchObject->m_searchType == Type26);
    m_list = GJListLayer::create(listView, title, color, 356.0f, listHeight, isType25or26);

    addChild(m_list);
    m_list->setPosition(
        (winSize.width - 356.0f) * 0.5f,
        (winSize.height - listHeight) * 0.5f - 10.0f + 5.0f + m_unkFloat
    );
}

void LevelBrowserLayer::setupPageInfo(std::string info, const char* key) {
    if (m_unk2 || !isCorrect(key)) {
        return;
    }

    // Parse info string "count:start:end"
    auto parts = cocos2d::CCArray::create();
    size_t pos = 0;
    size_t nextPos;

    while ((nextPos = info.find(':', pos)) != std::string::npos) {
        auto part = info.substr(pos, nextPos - pos);
        parts->addObject(cocos2d::CCString::create(part));
        pos = nextPos + 1;
    }
    if (pos < info.length()) {
        parts->addObject(cocos2d::CCString::create(info.substr(pos)));
    }

    if (parts->count() > 2) {
        auto countStr = static_cast<cocos2d::CCString*>(parts->objectAtIndex(0));
        auto startStr = static_cast<cocos2d::CCString*>(parts->objectAtIndex(1));
        auto endStr = static_cast<cocos2d::CCString*>(parts->objectAtIndex(2));

        m_itemCount = countStr->intValue();
        m_pageStartIdx = startStr->intValue();
        m_pageEndIdx = endStr->intValue();

        m_pageStartIdx = (m_pageStartIdx > 0) ? 1 : 0;

        m_leftArrow->setVisible(m_pageStartIdx != 0);
        m_rightArrow->setVisible((m_pageStartIdx + m_pageEndIdx) < m_itemCount);

        updateLevelsLabel();

        if (m_searchObject) {
            m_searchObject->m_total = m_itemCount;
        }

        m_pageBtn->setVisible(m_rightArrow->isVisible() || m_leftArrow->isVisible());
    }
}

void LevelBrowserLayer::updateLevelsLabel() {
    int endIdx = m_pageStartIdx + m_pageEndIdx;
    if (endIdx > (int)m_itemCount) {
        endIdx = m_itemCount;
    }

    auto str = cocos2d::CCString::createWithFormat(
        "%i to %i of %i",
        m_pageStartIdx + 1,
        endIdx,
        m_itemCount
    );
    m_countText->setString(str->getCString());
    m_countText->limitLabelWidth(100.0f, 0.6f, 0.0f);
    updatePageLabel();
}

void LevelBrowserLayer::updatePageLabel() {
    if (!m_pageText) {
        return;
    }

    int page = m_searchObject ? m_searchObject->m_page + 1 : 1;
    auto str = cocos2d::CCString::createWithFormat("%i", page);
    m_pageText->setString(str->getCString());
    m_pageText->limitLabelWidth(32.0f, 0.8f, 0.0f);

    if (m_folderText) {
        int folder = m_searchObject ? m_searchObject->m_folder : 0;
        auto folderStr = cocos2d::CCString::createWithFormat("%i", folder);
        m_folderText->setString(folderStr->getCString());
        m_folderText->limitLabelWidth(24.0f, 0.55f, 0.0f);
    }

    if (m_lastBtn) {
        m_lastBtn->setVisible(m_lastPage > 0);
    }
}

std::string LevelBrowserLayer::getSearchTitle() {
    std::string title = "Online Levels";

    if (!m_searchObject) {
        return title;
    }

    auto searchType = m_searchObject->m_searchType;

    if (searchType == SavedLevels || searchType == FavouriteLevels ||
        searchType == MyLists || searchType == FavouriteLists || searchType == MyLevels) {
        switch (searchType) {
            case SavedLevels: title = "Saved Levels"; break;
            case FavouriteLevels: title = "Favorite Levels"; break;
            case MyLists: title = "My Lists"; break;
            case FavouriteLists: title = "Favorite Lists"; break;
            default: title = "My Levels"; break;
        }

        if (m_searchObject->m_folder > 0) {
            auto folderName = GameLevelManager::sharedState()->getFolderName(
                m_searchObject->m_folder,
                m_searchObject->m_searchType == MyLevels
            );

            auto str = cocos2d::CCString::createWithFormat(
                " (%i%s)",
                m_searchObject->m_folder,
                folderName.empty() ? "" : (": " + folderName).c_str()
            );
            title += str->getCString();
        }
    } else if (m_searchObject->m_searchMode == 1) {
        title = "Online Lists";
    }

    if (!m_searchObject->m_searchQuery.empty()) {
        // Add search query to title if applicable
        // Logic from decomp
    }

    // Handle various search types
    if (searchType == UsersLevels) {
        // User levels logic
    } else if (searchType == MapPacks) {
        title = "Map Packs";
    } else if (searchType == MapPackOnClick) {
        auto mapPack = GameLevelManager::sharedState()->getSavedMapPack(
            GameLevelManager::sharedState()->m_mapPack
        );
        if (mapPack) {
            title = mapPack->m_packName;
        }
    } else if (searchType == Users) {
        title = "Find Users";
    } else if (searchType == SmartTemplates) {
        title = "Smart Templates";
    } else if (searchType == RecentMostLiked || searchType == HallOfFame ||
               searchType == FeaturedGDW || searchType == DailySafe ||
               searchType == WeeklySafe || searchType == EventSafe ||
               searchType == LevelListsOnClick || searchType == Type26 ||
               searchType == FeaturedLite || searchType == BonusLevels) {
        title = " ";
    }

    return title;
}

void LevelBrowserLayer::loadLevelsFailed(const char* key, int type) {
    if (isCorrect(key)) {
        m_circle->setVisible(false);
        if (!PlatformToolbox::isNetworkAvailable()) {
            m_noInternet->setVisible(true);
        }
        if (m_refreshBtn) {
            m_refreshBtn->setVisible(true);
        }
    }
}

void LevelBrowserLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    switch (layer->getTag()) {
        case 1: { // Delete All
            if (!btn2) return;

            auto gameLevelManager = GameLevelManager::sharedState();
            int folder = m_searchObject ? m_searchObject->m_folder : 0;
            auto savedLevels = gameLevelManager->getSavedLevels(false, folder);

            if (m_searchObject && !m_searchObject->m_searchQuery.empty()) {
                savedLevels = getItemsMatchingSearch(savedLevels, m_searchObject->m_searchQuery, m_searchObject);
            }

            for (int i = 0; i < savedLevels->count(); ++i) {
                auto level = static_cast<GJGameLevel*>(savedLevels->objectAtIndex(i));
                if (!level->m_isFavorite) {
                    GameLevelManager::sharedState()->deleteLevel(level);
                }
            }

            MusicDownloadManager::sharedState()->clearUnusedSongs();

            setKeypadEnabled(false);
            setKeyboardEnabled(false);
            onSaved(nullptr);
            break;
        }
        case 2: { // Remove All Favorites
            if (!btn2) return;

            if (m_searchObject && m_searchObject->m_searchType == FavouriteLists) {
                auto gameLevelManager = GameLevelManager::sharedState();
                int folder = m_searchObject ? m_searchObject->m_folder : 0;
                auto lists = gameLevelManager->getSavedLevelLists(folder);

                for (int i = 0; i < lists->count(); ++i) {
                    auto list = static_cast<GJLevelList*>(lists->objectAtIndex(i));
                    GameLevelManager::sharedState()->deleteLevelList(list);
                }

                setKeypadEnabled(false);
                setKeyboardEnabled(false);
                auto director = cocos2d::CCDirector::sharedDirector();
                auto scene = LevelBrowserLayer::scene(m_searchObject);
                auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
                director->replaceScene(transition);
            } else {
                auto gameLevelManager = GameLevelManager::sharedState();
                int folder = m_searchObject ? m_searchObject->m_folder : 0;
                auto levels = gameLevelManager->getSavedLevels(true, folder);

                if (m_searchObject && !m_searchObject->m_searchQuery.empty()) {
                    levels = getItemsMatchingSearch(levels, m_searchObject->m_searchQuery, m_searchObject);
                }

                for (int i = 0; i < levels->count(); ++i) {
                    auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));
                    level->m_isFavorite = false;
                }

                setKeypadEnabled(false);
                setKeyboardEnabled(false);
                onSaved(nullptr);
            }
            break;
        }
        case 5: { // Delete Selected
            if (btn2) {
                deleteSelected();
            }
            break;
        }
        case 6: { // Info
            if (btn2) {
                cocos2d::CCApplication::sharedApplication()->openURL("https://www.geometrydash.com/");
            }
            break;
        }
    }
}

void LevelBrowserLayer::deleteSelected() {
    if (!m_levels) return;

    auto selectedLevels = cocos2d::CCArray::create();

    for (int i = 0; i < m_levels->count(); ++i) {
        auto level = static_cast<GJGameLevel*>(m_levels->objectAtIndex(i));
        if (level && level->m_isSelected) {
            selectedLevels->addObject(level);
        }
    }

    if (selectedLevels->count() > 0) {
        for (int i = 0; i < selectedLevels->count(); ++i) {
            auto level = static_cast<GJGameLevel*>(selectedLevels->objectAtIndex(i));
            GameLevelManager::sharedState()->deleteLevel(level);
        }

        m_allSelected = false;
        if (m_allObjectsToggler) {
            m_allObjectsToggler->toggle(false);
        }
        loadPage(m_searchObject);
    }
}

void LevelBrowserLayer::onDeleteSelected(cocos2d::CCObject* sender) {
    if (!m_levels || m_levels->count() == 0) {
        auto alert = FLAlertLayer::create(nullptr, "Nothing here...", "No levels selected.", "OK", nullptr, nullptr);
        alert->show();
        return;
    }

    int selectedCount = 0;
    for (int i = 0; i < m_levels->count(); ++i) {
        auto level = static_cast<GJGameLevel*>(m_levels->objectAtIndex(i));
        if (level && level->m_isSelected) {
            selectedCount++;
        }
    }

    if (selectedCount > 0) {
        auto str = cocos2d::CCString::createWithFormat(
            "Are you sure you want to <cr>delete</c> the <cy>%i</c> selected <cg>levels</c>?",
            selectedCount
        );
        auto alert = FLAlertLayer::create(this, "Delete", str->getCString(), "Back", "Delete", nullptr, 380.0f, false, 0.0f, 1.0f);
        alert->setTag(5);
        alert->show();
    } else {
        auto alert = FLAlertLayer::create(nullptr, "Nothing here...", "No levels selected.", "OK", nullptr, nullptr);
        alert->show();
    }
}

void LevelBrowserLayer::onDeleteAll(cocos2d::CCObject* sender) {
    auto alert = FLAlertLayer::create(
        this,
        "Delete All",
        "Delete all saved online levels?\n<cg>Search and folder filters apply.</c>\n<cl>Favorite levels will not be deleted.</c>\n<cy>Stats and progress will be saved.</c>"
    );
    alert->setTag(1);
    alert->show();
}

void LevelBrowserLayer::onRemoveAllFavorites(cocos2d::CCObject* sender) {
    std::string message;
    if (m_searchObject && m_searchObject->m_searchType == FavouriteLists) {
        message = "Remove all lists from favorites?\n<cg>Search and folder filters apply.</c>";
    } else {
        message = "Remove all levels from favorites?\n<cg>Search and folder filters apply.</c>\n<cy>Levels will not be deleted.</c>";
    }

    auto alert = FLAlertLayer::create(this, "Remove Favorites", message.c_str());
    alert->setTag(2);
    alert->show();
}

void LevelBrowserLayer::onClearSearch(cocos2d::CCObject* sender) {
    if (m_searchObject) {
        m_searchObject->m_searchQuery = "";
        m_searchObject->m_page = 0;
        loadPage(m_searchObject);
    }
}

void LevelBrowserLayer::onSearch(cocos2d::CCObject* sender) {
    const char* title = (m_searchObject && m_searchObject->m_searchType == SmartTemplates) ?
        "Find Template" : "Find Level";

    auto popup = SetTextPopup::create(
        title,
        "Search",
        20,
        m_searchObject ? m_searchObject->m_searchQuery : "",
        "OK",
        true,
        60.0f
    );
    popup->m_delegate = this;
    if (m_isOverlay) {
        popup->m_ZOrder = getZOrder() + 1;
    }
    popup->show();
}

void LevelBrowserLayer::setTextPopupClosed(SetTextPopup* popup, std::string text) {
    if (m_searchObject) {
        m_searchObject->m_searchQuery = text;
        m_searchObject->m_page = 0;
        loadPage(m_searchObject);
    }
}

void LevelBrowserLayer::onGoToPage(cocos2d::CCObject* sender) {
    if (!m_searchObject) return;

    int currentPage = m_searchObject->m_page + 1;
    auto popup = SetIDPopup::create(currentPage, 1, 1000, "Go to Page", "Go", true, true, 60.0f, 0, 0);
    popup->setTag(3);
    popup->m_delegate = this;
    popup->show();
}

void LevelBrowserLayer::onGoToLastPage(cocos2d::CCObject* sender) {
    GameManager::sharedState()->m_currentLevelID = 0;
    auto pageObject = GJSearchObject::getPageObject(m_searchObject, m_lastPage);
    loadPage(pageObject);
}

void LevelBrowserLayer::onGoToFolder(cocos2d::CCObject* sender) {
    if (!m_searchObject) return;

    int folder = m_searchObject->m_folder;
    bool isMyLevels = m_searchObject->m_searchType == MyLevels;

    auto popup = SetFolderPopup::create(folder, isMyLevels, "Go to Folder");
    popup->setTag(4);
    popup->m_delegate = this;
    popup->show();
}

void LevelBrowserLayer::setIDPopupClosed(SetIDPopup* popup, int value) {
    if (popup->getTag() == 4) {
        if (value < 0) value = 0;
        if (value >= 1000) value = 999;

        if (m_searchObject) {
            GameManager::sharedState()->m_currentLevelID = 0;
            m_searchObject->m_folder = value;
            m_searchObject->m_page = 0;
            loadPage(m_searchObject);

            const char* varName = (m_searchObject->m_searchType == SavedLevels) ? "0092" : "0091";
            GameManager::sharedState()->setIntGameVariable(varName, value);
        }
    } else {
        int page = value - 1;
        if (page < m_lastPage && m_lastPage > 0) {
            m_lastPage = page;
        }

        if (m_searchObject && m_lastPage != m_searchObject->m_page) {
            GameManager::sharedState()->m_currentLevelID = 0;
            auto pageObject = GJSearchObject::getPageObject(m_searchObject, m_lastPage);
            loadPage(pageObject);
        }
    }
}

void LevelBrowserLayer::onNextPage(cocos2d::CCObject* sender) {
    GameManager::sharedState()->m_currentLevelID = 0;
    auto nextObject = GJSearchObject::getNextPageObject(m_searchObject);
    loadPage(nextObject);
}

void LevelBrowserLayer::onPrevPage(cocos2d::CCObject* sender) {
    GameManager::sharedState()->m_currentLevelID = 0;
    auto prevObject = GJSearchObject::getPrevPageObject(m_searchObject);
    loadPage(prevObject);
}

void LevelBrowserLayer::onRefresh(cocos2d::CCObject* sender) {
    if (m_searchObject) {
        auto key = m_searchObject->getKey();
        GameLevelManager::sharedState()->resetTimerForKey(key);
        loadPage(m_searchObject);
        if (m_refreshBtn) {
            m_refreshBtn->setVisible(false);
        }
    }
}

void LevelBrowserLayer::onBack(cocos2d::CCObject* sender) {
    if (m_isOverlay) {
        exitLayer(nullptr);
    } else {
        setKeypadEnabled(false);
        GameManager::sharedState()->safePopScene();
    }
}

void LevelBrowserLayer::exitLayer(cocos2d::CCObject* sender) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto touchDispatcher = director->getTouchDispatcher();
    touchDispatcher->unregisterForcePrio(this);
    removeFromParentAndCleanup(true);
}

void LevelBrowserLayer::keyBackClicked() {
    onBack(nullptr);
}

void LevelBrowserLayer::onEnter() {
    cocos2d::CCLayer::onEnter();

    if (!m_unk) {
        if (m_searchObject) {
            auto searchType = m_searchObject->m_searchType;
            if (searchType == Type25 || searchType == Type26) {
                setupLevelBrowser(nullptr);
            } else if (searchType == RecentMostLiked || searchType == MapPacks) {
                loadPage(m_searchObject);
            } else {
                loadPage(m_searchObject);
            }
        }
        m_unk = true;
    }
}

void LevelBrowserLayer::onNew(cocos2d::CCObject* sender) {
    if (m_searchObject) {
        auto searchType = m_searchObject->m_searchType;
        if (searchType == SmartTemplates) {
            createNewSmartTemplate(sender);
            return;
        } else if (searchType == MyLists) {
            createNewList(sender);
            return;
        }
    }
    createNewLevel(sender);
}

void LevelBrowserLayer::createNewLevel(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    auto gameLevelManager = GameLevelManager::sharedState();
    auto newLevel = gameLevelManager->createNewLevel();
    GameLevelManager::sharedState()->m_returnToLocalLevels = true;

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = EditLevelLayer::scene(newLevel);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);
}

void LevelBrowserLayer::createNewList(cocos2d::CCObject* sender) {
    if (m_isOverlay) {
        auto popup = ShareCommentLayer::create("List Name", 25, CommentType::ListName, false, "");
        popup->m_delegate = this;
        popup->show();
    } else {
        setKeypadEnabled(false);
        setKeyboardEnabled(false);

        auto gameLevelManager = GameLevelManager::sharedState();
        auto newList = gameLevelManager->createNewLevelList();
        GameLevelManager::sharedState()->m_returnToLocalLevels = true;

        auto director = cocos2d::CCDirector::sharedDirector();
        auto scene = LevelListLayer::scene(newList);
        auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        director->replaceScene(transition);
    }
}

void LevelBrowserLayer::shareCommentClosed(std::string text, ShareCommentLayer* layer) {
    if (layer->m_commentType == CommentType::ListName) {
        auto gameLevelManager = GameLevelManager::sharedState();
        auto newList = gameLevelManager->createNewLevelList();
        GameLevelManager::sharedState()->m_returnToLocalLevels = true;
        newList->m_listName = text;
        loadPage(m_searchObject);
    }
}

void LevelBrowserLayer::createNewSmartTemplate(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    auto gameLevelManager = GameLevelManager::sharedState();
    auto template_ = gameLevelManager->createSmartTemplate();

    auto popup = SetupSmartTemplateLayer::create(template_);
    popup->show();

    onBack(nullptr);
}

void LevelBrowserLayer::onSaved(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    auto searchType = SavedLevels;
    auto searchObject = GJSearchObject::create(searchType);

    if (m_searchObject) {
        searchObject->m_folder = m_searchObject->m_folder;
        searchObject->m_searchQuery = m_searchObject->m_searchQuery;
    }

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = LevelBrowserLayer::scene(searchObject);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);
}

void LevelBrowserLayer::onFavorites(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    auto searchType = FavouriteLevels;
    auto searchObject = GJSearchObject::create(searchType);

    if (m_searchObject) {
        searchObject->m_folder = m_searchObject->m_folder;
        searchObject->m_searchQuery = m_searchObject->m_searchQuery;
    }

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = LevelBrowserLayer::scene(searchObject);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);
}

void LevelBrowserLayer::onLocalMode(cocos2d::CCObject* sender) {
    if (!m_searchObject) return;

    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    GJSearchObject* searchObject = nullptr;

    if (m_searchObject->m_searchType == MyLevels) {
        GameManager::sharedState()->m_localSearchType = MyLists;
        searchObject = GJSearchObject::create(MyLists);
        searchObject->m_searchMode = 1;
    } else {
        GameManager::sharedState()->m_localSearchType = MyLevels;
        searchObject = GJSearchObject::create(MyLevels);
    }

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = LevelBrowserLayer::scene(searchObject);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);
}

void LevelBrowserLayer::onSavedMode(cocos2d::CCObject* sender) {
    if (!m_searchObject) return;

    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    GJSearchObject* searchObject = nullptr;

    if (m_searchObject->m_searchType == SavedLevels) {
        GameManager::sharedState()->m_savedSearchType = FavouriteLists;
        searchObject = GJSearchObject::create(FavouriteLists);
        searchObject->m_searchMode = 1;
    } else {
        GameManager::sharedState()->m_savedSearchType = SavedLevels;
        searchObject = GJSearchObject::create(SavedLevels);
    }

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = LevelBrowserLayer::scene(searchObject);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);
}

void LevelBrowserLayer::onMyOnlineLevels(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    GJSearchObject* searchObject = nullptr;

    if (m_searchObject && m_searchObject->m_searchType == MyLists) {
        auto accManager = GJAccountManager::sharedState();
        auto userID = accManager->m_accountID - accManager->m_unkInt1;
        auto userIDStr = GameToolbox::intToString(userID);

        searchObject = GJSearchObject::create(UsersLevels);
        searchObject->m_searchMode = 1;
    } else {
        auto gameManager = GameManager::sharedState();
        auto userID = gameManager->m_playerUserID.random - gameManager->m_playerUserID.seed;
        auto userIDStr = GameToolbox::intToString(userID);

        searchObject = GJSearchObject::create(UsersLevels);
    }

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = LevelBrowserLayer::scene(searchObject);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);
}

void LevelBrowserLayer::onToggleAllObjects(cocos2d::CCObject* sender) {
    m_allSelected = !m_allSelected;

    if (m_levels && m_levels->count() > 0) {
        for (int i = 0; i < m_levels->count(); ++i) {
            auto level = static_cast<GJGameLevel*>(m_levels->objectAtIndex(i));
            if (level) {
                level->m_isSelected = m_allSelected;
            }
        }

        if (m_list && m_list->m_listView) {
            static_cast<CustomListView*>(m_list->m_listView)->reloadAll();
        }
    }
}

void LevelBrowserLayer::reloadAllObjects() {
    if (m_list && m_list->m_listView) {
        static_cast<CustomListView*>(m_list->m_listView)->reloadAll();
    }
}

cocos2d::CCScene* LevelBrowserLayer::scene(GJSearchObject* object) {
    auto scene = cocos2d::CCScene::create();
    auto layer = LevelBrowserLayer::create(object);
    scene->addChild(layer);
    return scene;
}

void LevelBrowserLayer::show() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto runningScene = director->getRunningScene();

    if (!m_scene) {
        int highestZ = runningScene->getHighestChildZ();
        m_zOffset = (highestZ > 104) ? highestZ + 1 : 105;
        m_scene = runningScene;
    }

    if (m_zOffset == 0) {
        m_zOffset = 105;
    }

    m_scene->addChild(this, m_zOffset);
}

void LevelBrowserLayer::registerWithTouchDispatcher() {
    if (m_isOverlay) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto touchDispatcher = director->getTouchDispatcher();
        touchDispatcher->addTargetedDelegate(this, -500, true);
    } else {
        cocos2d::CCLayer::registerWithTouchDispatcher();
    }
}

bool LevelBrowserLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    return true;
}

void LevelBrowserLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {}
void LevelBrowserLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {}
void LevelBrowserLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {}

void LevelBrowserLayer::setSearchObject(GJSearchObject* object) {
    if (m_searchObject != object) {
        if (object) {
            object->retain();
        }
        if (m_searchObject) {
            m_searchObject->release();
        }
        m_searchObject = object;
    }
}

bool LevelBrowserLayer::isCorrect(const char* key) {
    if (!m_searchObject) return false;

    auto searchKey = m_searchObject->getKey();
    return strcmp(searchKey, key) == 0;
}

cocos2d::CCArray* LevelBrowserLayer::updateResultArray(cocos2d::CCArray* results) {
    return results;
}

void LevelBrowserLayer::onInfo(cocos2d::CCObject* sender) {
    std::string title = "Info";
    std::string message = "<cg>Green</c> users are unregistered.\n<cy>Gold</c> users are unique registered.\n<cl>Blue</c> songs are official.\n<cp>Purple</c> songs are custom.";

    if (m_searchObject) {
        if (m_searchObject->m_searchMode == 1) {
            title = "Lists";
            message = "A <co>list</c> may have rewards if it is rated by RobTop. The difficulty face is manually set by the creator of the list.\nUsernames are <cy>gold</c> for registered users and <cg>green</c> for unregistered users. A song is <cl>blue</c> if it is official and <cp>pink</c> if it is custom.";

            if (m_searchObject->m_searchType == RecentMostLiked) {
                title = "Top Lists";
                message = "These <co>lists</c> have been rated by RobTop. The rewards for each one are also decided by him and are variable from list to list. Note that rated lists can only be <cy>Featured</c> and do not reward Creator Points.";
            }
        } else {
            title = "Levels";
            message = "A <co>level</c> rewards <cy>Stars</c> if it is rated by RobTop. For unrated levels, the difficulty face is set by community votes on each level's page. This is also how <cr>Demon</c> difficulties are decided.\nUsernames are <cy>gold</c> for registered users and <cg>green</c> for unregistered users. A song is <cl>blue</c> if it is official and <cp>pink</c> if it is custom.";

            switch (m_searchObject->m_searchType) {
                case RecentMostLiked:
                    title = "Featured";
                    message = "Levels on this list have been rated and featured by RobTop. The extra visual around the difficulty face indicates how much he likes a specific level. In order of increasing quality:\n<cy>Featured</c>, <co>Epic</c>, <cp>Legendary</c>, and <cj>Mythic</c>.";
                    break;
                case MapPacks:
                    title = "Map Packs";
                    message = "<cg>Map Packs</c> are collections of 3 rated levels of similar difficulty set by RobTop. Once completed, they reward <cy>Stars</c> and <cl>Secret Coins</c>.";
                    break;
                case FeaturedLite:
                    title = "Weekly Levels";
                    message = "New <cg>levels</c> are released every <cy>week</c>.\n<cl>Check back later for more updates!</c>";
                    break;
                case BonusLevels:
                    title = "Bonus Levels";
                    message = "New <cg>levels</c> are released from time to time.\n<cl>Check back later for more updates!</c>";
                    break;
            }
        }
    }

    auto alert = FLAlertLayer::create(this, title.c_str(), message.c_str());
    alert->setTag(6);
    alert->show();
}

void LevelBrowserLayer::onHelp(cocos2d::CCObject* sender) {}

LevelBrowserLayer::~LevelBrowserLayer() {
    if (m_circle) {
        m_circle->release();
    }
}