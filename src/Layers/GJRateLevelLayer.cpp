#include "Layers/GJRateLevelLayer.h"
#include "Models/GJGameLevel.h"
#include "Sprites/GJDifficultySprite.h"
#include "UI/LoadingCircle.h"
#include "Popups/UploadActionPopup.h"
#include "GameLevelManager.h"
#include "Utils/GameToolbox.h"
#include "Sprites/ButtonSprite.h"
#include "Layers/FLAlertLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GJRateLevelLayer* GJRateLevelLayer::create(GJGameLevel* level) {
    auto layer = new GJRateLevelLayer();
    if (layer && layer->init(level)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool GJRateLevelLayer::init(GJGameLevel* level) {
    if (!level || !SetupTriggerPopup::init(nullptr, nullptr, 420.0f, 290.0f, 1)) {
        return false;
    }
    
    SetupTriggerPopup::preSetup();
    SetupTriggerPopup::addTitle("Suggest Rating");
    
    m_level = level;
    m_level->retain();
    m_stars = -1;
    m_featureState = GJFeatureState::None;
    m_uploadFinished = false;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_starButtons = cocos2d::CCArray::create();
    m_starButtons->retain();
    
    auto row1 = cocos2d::CCArray::create();
    auto row2 = cocos2d::CCArray::create();
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    m_coinSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
    m_coinSprite->setScale(1.4f);
    auto coinItem = CCMenuItemSpriteExtra::create(m_coinSprite, this, menu_selector(GJRateLevelLayer::onToggleCoins));
    m_buttonMenu->addChild(coinItem);
    coinItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(
        -(m_width * 0.5f - centerX),
        m_height * 0.5f + centerY
    )));
    m_coinsToggled = false;
    m_coinSprite->setColor(cocos2d::ccc3(50, 50, 50));
    onToggleCoins(nullptr);
    
    for (int i = 1; i <= 10; ++i) {
        auto btn = getStarsButton(i, menu_selector(GJRateLevelLayer::selectRating), m_buttonMenu, 0.75f);
        m_starButtons->addObject(btn);
        btn->setTag(i);
        
        if (i <= 5) {
            row1->addObject(btn);
        } else {
            row2->addObject(btn);
        }
    }
    
    cocos2d::CCPoint row1Pos = m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 90.0f, centerY + 75.0f + 19.55f));
    GameToolbox::alignItemsHorisontally(row1, 13.6f, row1Pos);
    
    cocos2d::CCPoint row2Pos = m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 90.0f, centerY + 75.0f - 19.55f));
    GameToolbox::alignItemsHorisontally(row2, 13.6f, row2Pos);
    
    cocos2d::CCPoint diffPos(centerX - 90.0f + 140.0f, centerY + 75.0f);
    
    m_difficultySprite = GJDifficultySprite::create(0, DifficultyIconType::Long);
    m_difficultySprite->setScale(1.2f);
    auto diffItem = CCMenuItemSpriteExtra::create(m_difficultySprite, this, menu_selector(GJRateLevelLayer::onFeature));
    m_buttonMenu->addChild(diffItem);
    diffItem->setPosition(m_buttonMenu->convertToNodeSpace(diffPos));
    
    SetupTriggerPopup::createValueControlAdvanced(
        this,
        -1,
        "Rank:",
        cocos2d::CCPoint(0.75f, 0.0f),
        0.0f,
        diffPos + cocos2d::CCPoint(90.0f, 0.0f),
        ValueType::Float,
        2,
        4,
        1.0f,
        0.0f,
        1,
        0,
        ControlType::ChatLabel,
        0,
        2
    );
    
    SetupTriggerPopup::updateValue(-1, 1.0f);
    toggleGroup(1, false);
    
    row1->removeAllObjects();
    
    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square02.png");
    bg->setColor(cocos2d::ccc3(0, 0, 0));
    bg->setOpacity(75);
    bg->setContentSize(cocos2d::CCSize(280.0f, 110.0f));
    m_mainLayer->addChild(bg, 1);
    m_center = cocos2d::CCPoint(centerX + 50.0f, centerY - 32.0f);
    bg->setPosition(m_center);
    
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->setScale(0.7f);
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->retain();
    m_loadingCircle->m_sprite->setPosition(bg->getPosition());
    m_loadingCircle->show();
    
    auto unrateBtn = ButtonSprite::create("Unrate", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_05.png", nullptr, 25.0f);
    SetupTriggerPopup::addCustomButton(this, -2, unrateBtn, cocos2d::CCPoint(centerX - 150.0f, m_center.y + 40.0f));
    
    auto rankOnlyBtn = ButtonSprite::create("RankOnly", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_05.png", nullptr, 25.0f);
    SetupTriggerPopup::addCustomButton(this, -4, rankOnlyBtn, cocos2d::CCPoint(centerX - 150.0f, m_center.y));
    
    auto cancelBtn = ButtonSprite::create("Cancel");
    auto cancelItem = CCMenuItemSpriteExtra::create(cancelBtn, this, menu_selector(GJRateLevelLayer::onClose));
    m_buttonMenu->addChild(cancelItem);
    
    auto submitBtn = ButtonSprite::create("Submit");
    m_submitButton = CCMenuItemSpriteExtra::create(submitBtn, this, menu_selector(GJRateLevelLayer::onRate));
    m_buttonMenu->addChild(m_submitButton);
    
    cocos2d::CCPoint btnPos = m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX, 25.0f - (m_height * 0.5f - centerY)));
    cocos2d::CCArray* btnArray = cocos2d::CCArray::create();
    btnArray->addObject(cancelItem);
    btnArray->addObject(m_submitButton);
    GameToolbox::alignItemsHorisontally(btnArray, 15.0f, btnPos);
    
    selectRating(nullptr);
    
    GameLevelManager::sharedState()->m_levelRateInfoDelegate = this;
    GameLevelManager::sharedState()->getLevelRateInfo(m_level->m_levelID.random - m_level->m_levelID.seed);
    
    SetupTriggerPopup::postSetup();
    
    return true;
}

void GJRateLevelLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2 || layer->getTag() != -3) {
        return;
    }
    
    auto manager = GameLevelManager::sharedState();
    if (manager->unrateLevelAdmin(m_level->m_levelID.random - m_level->m_levelID.seed)) {
        manager->m_uploadActionDelegate = this;
        
        if (m_uploadPopup) {
            m_uploadPopup->m_delegate = nullptr;
        }
        
        auto popup = UploadActionPopup::create(this, "Unrating level...");
        m_uploadPopup = popup;
        popup->show();
    }
}

void GJRateLevelLayer::keyBackClicked() {
    onClose(nullptr);
}

void GJRateLevelLayer::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    m_uploadPopup = nullptr;
    if (m_uploadFinished) {
        onClose(nullptr);
    }
}

void GJRateLevelLayer::onCustomButton(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    
    if (tag == -4) {
        int rank = static_cast<int>(getValue(-1));
        if (rank > 0 && m_featureState > GJFeatureState::None) {
            auto manager = GameLevelManager::sharedState();
            if (manager->updateLevelRankAdmin(m_level->m_levelID.random - m_level->m_levelID.seed, rank)) {
                manager->m_uploadActionDelegate = this;
                
                if (m_uploadPopup) {
                    m_uploadPopup->m_delegate = nullptr;
                }
                
                auto popup = UploadActionPopup::create(this, "Updating rank...");
                m_uploadPopup = popup;
                popup->show();
            }
        }
    } else if (tag == -2) {
        auto alert = FLAlertLayer::create(this, "Unrate Level", "Do you want to unrate this level?", "Back", "Delete", 380.0f);
        alert->setTag(-3);
        alert->show();
    }
}

void GJRateLevelLayer::onFeature(cocos2d::CCObject* sender) {
    if (m_featureState < GJFeatureState::Legendary) {
        m_featureState = static_cast<GJFeatureState>(static_cast<int>(m_featureState) + 1);
    } else {
        m_featureState = GJFeatureState::None;
    }
    
    m_difficultySprite->updateFeatureState(m_featureState);
    toggleGroup(1, m_featureState != GJFeatureState::None);
}

void GJRateLevelLayer::onRate(cocos2d::CCObject* sender) {
    if (m_stars == -1) {
        return;
    }
    
    auto manager = GameLevelManager::sharedState();
    if (manager->suggestLevelStars(
        m_level->m_levelID.random - m_level->m_levelID.seed,
        m_stars,
        m_featureState
    )) {
        manager->m_uploadActionDelegate = this;
        
        if (m_uploadPopup) {
            m_uploadPopup->m_delegate = nullptr;
        }
        
        auto popup = UploadActionPopup::create(this, "Sending rating...");
        m_uploadPopup = popup;
        popup->show();
    }
}

void GJRateLevelLayer::onToggleCoins(cocos2d::CCObject* sender) {
    m_coinsToggled = !m_coinsToggled;
    m_coinSprite->setColor(m_coinsToggled ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(50, 50, 50));
}

void GJRateLevelLayer::rateInfoFailed(int id, int response) {
    if (id == m_level->m_levelID.random - m_level->m_levelID.seed) {
        m_loadingCircle->setVisible(false);
    }
}

void GJRateLevelLayer::rateInfoFinished(int id, const std::string& info) {
    if (id == m_level->m_levelID.random - m_level->m_levelID.seed) {
        m_loadingCircle->setVisible(false);
        setupRateInfo(info);
    }
}

void GJRateLevelLayer::selectRating(cocos2d::CCObject* sender) {
    m_stars = sender ? sender->getTag() : -1;
    
    for (int i = 0; i < m_starButtons->count(); ++i) {
        auto obj = m_starButtons->objectAtIndex(i);
        auto btn = static_cast<CCMenuItemSpriteExtra*>(obj);
        bool selected = (m_stars == btn->getTag());
        ButtonSprite::updateBGImage(btn->getNormalImage(), selected ? "GJ_button_01.png" : "GJ_button_04.png");
    }
    
    auto submitImage = m_submitButton->getNormalImage();
    if (m_stars == -1) {
        ButtonSprite::setColor(submitImage, cocos2d::ccc3(150, 150, 150));
    } else {
        ButtonSprite::setColor(submitImage, cocos2d::ccc3(255, 255, 255));
    }
    
    int diff = 0;
    if (m_stars <= 1) {
        diff = (m_stars == 1) ? -1 : 0;
    } else if (m_stars == 2) {
        diff = 1;
    } else if (m_stars == 3) {
        diff = 2;
    } else if (m_stars <= 5) {
        diff = 3;
    } else if (m_stars <= 7) {
        diff = 4;
    } else if (m_stars <= 9) {
        diff = 5;
    } else if (m_stars == 10) {
        diff = 6;
    } else {
        diff = 0;
    }
    
    m_difficultySprite->updateDifficultyFrame(diff, DifficultyIconType::Short);
}

void GJRateLevelLayer::setupRateInfo(const std::string& info) {
    auto parts = splitString(info, ":");
    if (parts.empty()) return;
    
    auto starData = splitString(parts[0], ",");
    int maxStars = 0;
    for (const auto& s : starData) {
        maxStars = std::max(maxStars, std::stoi(s));
    }
    
    float scale = 10.0f;
    if (static_cast<float>(maxStars * 10) > 100.0f) {
        scale = 100.0f / static_cast<float>(maxStars);
    }
    
    int xOffset = -120;
    for (size_t i = 0; i < starData.size(); ++i) {
        int count = std::stoi(starData[i]);
        cocos2d::CCPoint pos = m_center + cocos2d::CCPoint(static_cast<float>(xOffset), -42.0f);
        
        cocos2d::ccColor3B innerColor = cocos2d::ccc3(170, 150, 100);
        cocos2d::ccColor3B outerColor = cocos2d::ccc3(0, 0, 0);
        std::string label = std::to_string(i + 1);
        float height = scale;
        int countVal = count;
        
        if (count <= 0) {
            innerColor = cocos2d::ccc3(50, 50, 50);
            outerColor = cocos2d::ccc3(30, 30, 30);
            label = "";
            if (scale < 10.0f) {
                height = scale;
                countVal = 1;
            } else {
                height = scale * 0.5f;
                countVal = 1;
                pos.y -= height * 0.5f;
            }
        } else {
            switch (i) {
                case 0: innerColor = cocos2d::ccc3(200, 100, 255); break;
                case 1: innerColor = cocos2d::ccc3(100, 200, 255); break;
                case 2: innerColor = cocos2d::ccc3(100, 255, 100); break;
                case 3: innerColor = cocos2d::ccc3(255, 200, 100); break;
                case 4: innerColor = cocos2d::ccc3(255, 100, 100); break;
                case 5: innerColor = cocos2d::ccc3(255, 50, 50); break;
                case 6: innerColor = cocos2d::ccc3(255, 200, 50); break;
                case 7: innerColor = cocos2d::ccc3(200, 200, 50); break;
                case 8: innerColor = cocos2d::ccc3(100, 200, 100); break;
                case 9: innerColor = cocos2d::ccc3(100, 200, 200); break;
                default: break;
            }
        }
        
        createStatPillar(10, static_cast<int>(height), countVal, innerColor, outerColor, pos, label);
        xOffset += 15;
    }
    
    auto divider = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    divider->setScaleX(0.06f);
    divider->setScaleY(110.0f / divider->getContentSize().height);
    divider->setColor(cocos2d::ccc3(100, 100, 100));
    divider->setOpacity(200);
    m_mainLayer->addChild(divider, 3);
    divider->setPosition(m_center + cocos2d::CCPoint(38.0f, 0.0f));
    
    if (parts.size() > 1) {
        auto featureData = splitString(parts[1], ",");
        int maxFeature = 0;
        for (const auto& s : featureData) {
            maxFeature = std::max(maxFeature, std::stoi(s));
        }
        
        float featureScale = 10.0f;
        if (static_cast<float>(maxFeature * 10) > 100.0f) {
            featureScale = 100.0f / static_cast<float>(maxFeature);
        }
        
        int xPos = 60;
        for (size_t i = 0; i < featureData.size(); ++i) {
            int count = std::stoi(featureData[i]);
            cocos2d::CCPoint pos = m_center + cocos2d::CCPoint(static_cast<float>(xPos), -42.0f);
            
            cocos2d::ccColor3B innerColor = cocos2d::ccc3(170, 150, 100);
            cocos2d::ccColor3B outerColor = cocos2d::ccc3(0, 0, 0);
            std::string label;
            float height = featureScale;
            int countVal = count;
            
            if (count <= 0) {
                innerColor = cocos2d::ccc3(50, 50, 50);
                outerColor = cocos2d::ccc3(30, 30, 30);
                label = "";
                if (featureScale < 10.0f) {
                    height = featureScale;
                    countVal = 1;
                } else {
                    height = featureScale * 0.5f;
                    countVal = 1;
                    pos.y -= height * 0.5f;
                }
            } else {
                switch (i) {
                    case 0: innerColor = cocos2d::ccc3(255, 255, 255); label = "F"; break;
                    case 1: innerColor = cocos2d::ccc3(255, 200, 0); label = "E"; break;
                    case 2: innerColor = cocos2d::ccc3(0, 255, 0); label = "L"; break;
                    case 3: innerColor = cocos2d::ccc3(0, 150, 255); label = "M"; break;
                    default: label = "R"; break;
                }
            }
            
            createStatPillar(10, static_cast<int>(height), countVal, innerColor, outerColor, pos, label);
            xPos += 15;
        }
    }
    
    auto infoLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    std::string statsText;
    if (m_level->m_starRatings <= 0) {
        statsText = "Req: " + std::to_string(m_level->m_starsRequested);
    } else {
        float avg = static_cast<float>(m_level->m_starRatingsSum) / m_level->m_starRatings;
        statsText = "R: " + std::to_string(m_level->m_starRatings) +
                   " (" + std::to_string(m_level->m_minStarRatings) + "- / " +
                   std::to_string(m_level->m_maxStarRatings) + "+)\n" +
                   std::to_string(avg) + " | +" +
                   std::to_string(static_cast<float>(m_level->m_starRatingsSum - m_level->m_minStarRatings) / (m_level->m_starRatings - m_level->m_minStarRatings)) +
                   " | -" +
                   std::to_string(static_cast<float>(m_level->m_starRatingsSum - m_level->m_maxStarRatings * 10) / (m_level->m_starRatings - m_level->m_maxStarRatings)) +
                   " | +-" +
                   std::to_string(static_cast<float>((m_level->m_starRatingsSum - m_level->m_maxStarRatings * 10) - m_level->m_minStarRatings) /
                                 ((m_level->m_starRatings - m_level->m_maxStarRatings) - m_level->m_minStarRatings)) +
                   "\nReq: " + std::to_string(m_level->m_starsRequested);
    }
    infoLabel->setString(statsText.c_str());
    infoLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
    infoLabel->setScale(0.6f);
    infoLabel->setColor(cocos2d::ccc3(150, 150, 150));
    infoLabel->setOpacity(225);
    m_mainLayer->addChild(infoLabel, 2);
    infoLabel->setPosition(m_center + cocos2d::CCPoint(-130.0f, 48.0f));
}

void GJRateLevelLayer::uploadActionFailed(int id, int response) {
    if (id == m_level->m_levelID.random - m_level->m_levelID.seed && m_uploadPopup) {
        m_uploadPopup->showFailMessage("Failed. Please try again later.");
        m_uploadPopup->m_delegate = nullptr;
        m_uploadPopup = nullptr;
    }
}

void GJRateLevelLayer::uploadActionFinished(int id, int response) {
    if (id == m_level->m_levelID.random - m_level->m_levelID.seed && m_uploadPopup) {
        m_uploadFinished = true;
        m_uploadPopup->showSuccessMessage("Rating submitted!");
        m_uploadPopup->m_delegate = nullptr;
        m_uploadPopup = nullptr;
    }
}

GJRateLevelLayer::~GJRateLevelLayer() {
    if (m_level) {
        m_level->release();
    }
    if (m_starButtons) {
        m_starButtons->release();
    }
    if (m_loadingCircle) {
        m_loadingCircle->release();
    }
}