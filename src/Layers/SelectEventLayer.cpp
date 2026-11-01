#include "Layers/SelectEventLayer.h"
#include "Popups/Setup/SetupEventLinkPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Utils/GameToolbox.h"
#include "Layers/GJBaseGameLayer.h"
#include "Layers/FLAlertLayer.h"

SelectEventLayer* SelectEventLayer::create(SetupEventLinkPopup* popup, std::set<int>* eventIDs) {
    SelectEventLayer* layer = new SelectEventLayer();
    if (layer && layer->init(popup, eventIDs)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SelectEventLayer::init(SetupEventLinkPopup* popup, std::set<int>* eventIDs) {
    if (!SetupTriggerPopup::init(0, 0, 380.0f, 280.0f, 2)) {
        return false;
    }

    SetupTriggerPopup::preSetup();
    SetupTriggerPopup::addTitle("Select Event");
    SetupTriggerPopup::addCloseButton("OK");

    m_eventLinkPopup = popup;
    m_eventIDs = *eventIDs;

    FLAlertLayer::incrementForcePrio();
    m_noElasticity = true;
    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addToggle(1, "Triggered on very small distances. Like walking down small stairs.");
    addToggle(2, "Triggered on landing with very low velocity.");
    addToggle(3, "Triggered on landing with low velocity.");
    addToggle(4, "Triggered on landing with normal velocity.");
    addToggle(5, "Triggered on landing with high velocity.");
    addToggle(6, "Triggered when the player head collides with a block.");
    addToggle(7, "");
    addToggle(8, "");
    addToggle(9, "");
    addToggle(10, "");
    addToggle(11, "");
    addToggle(12, "");
    addToggle(13, "");
    addToggle(14, "");
    addToggle(15, "");
    addToggle(16, "");
    addToggle(17, "");
    addToggle(18, "");
    addToggle(19, "");
    addToggle(20, "");
    addToggle(21, "");
    addToggle(22, "");
    addToggle(23, "");
    addToggle(24, "");
    addToggle(25, "");
    addToggle(26, "");
    addToggle(27, "");
    addToggle(28, "");
    addToggle(29, "");
    addToggle(30, "");
    addToggle(31, "");
    addToggle(32, "");
    addToggle(33, "");
    addToggle(34, "");
    addToggle(35, "");
    addToggle(36, "");
    addToggle(37, "");
    addToggle(38, "");
    addToggle(39, "");
    addToggle(40, "");
    addToggle(41, "");
    addToggle(42, "");
    addToggle(43, "");
    addToggle(44, "");
    addToggle(45, "");
    addToggle(46, "");
    addToggle(47, "");
    addToggle(48, "");
    addToggle(49, "");
    addToggle(50, "");
    addToggle(51, "");
    addToggle(52, "");
    addToggle(53, "");
    addToggle(54, "");
    addToggle(55, "");
    addToggle(56, "");
    addToggle(57, "");
    addToggle(58, "");
    addToggle(59, "");
    addToggle(60, "");
    addToggle(61, "");
    addToggle(62, "");
    addToggle(63, "");
    addToggle(64, "");
    addToggle(65, "");
    addToggle(66, "");
    addToggle(67, "");
    addToggle(68, "");
    addToggle(69, "");
    addToggle(70, "");
    addToggle(71, "");
    addToggle(72, "");
    addToggle(73, "");
    addToggle(74, "");
    addToggle(75, "");
    addToggle(76, "");
    addToggle(77, "");
    addToggle(78, "");

    SetupTriggerPopup::createPageButtons(m_width * 0.5f + 15.0f, 1);

    if (!m_eventIDs.empty()) {
        onCustomToggleTriggerValue(nullptr);
    }

    SetupTriggerPopup::goToPage(0, false);

    return true;
}

void SelectEventLayer::addToggle(int id, std::string info) {
    int page = m_toggleCount / 12;
    m_toggleCount++;
    if (page >= m_maxPage) {
        m_maxPage = page;
    }

    nextPosition();

    bool isSelected = (m_eventIDs.find(id) != m_eventIDs.end());
    std::string eventName = GJBaseGameLayer::gameEventToString((GJGameEvent)id);

    SetupTriggerPopup::createCustomToggleValueControl(
        (GJGameEvent)id, isSelected, true, eventName, cocos2d::CCPoint(0, 0), false, page);

    cocos2d::CCLabelBMFont* idLabel = cocos2d::CCLabelBMFont::create(
        GameToolbox::intToString(id).c_str(), "bigFont.fnt");
    idLabel->setScale(0.6f);
    idLabel->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    m_mainLayer->addChild(idLabel, 10);
    idLabel->setColor(ccc3(150, 150, 150));
    idLabel->setOpacity(150);
    idLabel->setPosition(cocos2d::CCPoint(0, 0));
    SetupTriggerPopup::addObjectToPage(idLabel, page);

    if (!info.empty()) {
        m_eventInfos[id] = info;

        cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSprite->setScale(0.5f);
        CCMenuItemSpriteExtra* infoButton = CCMenuItemSpriteExtra::create(
            infoSprite, nullptr, menu_selector(SelectEventLayer::onInfo));
        infoButton->setSizeMult(2.0f);
        infoButton->setTag(id);
        m_buttonMenu->addChild(infoButton);
        infoButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
            cocos2d::CCPoint(0, 0) + cocos2d::CCPoint(-26.0f, 0.0f)));
        SetupTriggerPopup::addObjectToPage(infoButton, page);
    }
}

cocos2d::CCPoint SelectEventLayer::nextPosition() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint leftPos = cocos2d::CCPoint(winSize.width * 0.5f - 140.0f, winSize.height * 0.5f + 90.0f);
    cocos2d::CCPoint rightPos = cocos2d::CCPoint(winSize.width * 0.5f + 40.0f, leftPos.y);

    int idx = (m_toggleCount - 1) % 12;
    cocos2d::CCPoint pos = (idx >= 6) ? rightPos : leftPos;
    pos.y -= 34.0f * (idx % 6);

    return pos;
}

void SelectEventLayer::keyBackClicked() {
    onClose(nullptr);
}

void SelectEventLayer::onClose(cocos2d::CCObject* sender) {
    if (m_eventsChanged) {
        if (m_eventLinkPopup) {
            m_eventLinkPopup->updateEventIDs(&m_eventIDs);
        }
    }
    SetupTriggerPopup::onClose(sender);
}

void SelectEventLayer::onCustomToggleTriggerValue(cocos2d::CCObject* sender) {
    if (sender) {
        m_eventsChanged = true;
        int tag = sender->getTag();

        if (m_eventIDs.find(tag) != m_eventIDs.end()) {
            m_eventIDs.erase(tag);
        } else {
            m_eventIDs.insert(tag);
        }
    }

    if (!m_customValueToggles) {
        return;
    }

    cocos2d::CCDictElement* element = m_customValueToggles->m_pElements;
    while (element) {
        CCMenuItemToggler* toggler = (CCMenuItemToggler*)element->m_pObject;
        int tag = toggler->getTag();

        if (m_eventIDs.find(tag) != m_eventIDs.end()) {
            toggler->toggle(true);
        } else {
            toggler->toggle(false);
        }

        element = (cocos2d::CCDictElement*)element->hh.next;
    }
}

void SelectEventLayer::onInfo(cocos2d::CCObject* sender) {
    int tag = sender->getTag();

    auto it = m_eventInfos.find(tag);
    if (it == m_eventInfos.end()) {
        return;
    }

    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Info", it->second.c_str());
    alert->show();
}

SelectEventLayer::~SelectEventLayer() {
}