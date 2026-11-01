#include "LevelOptionsLayer.h"
#include "LevelSettingsObject.h"
#include "GJOptionsLayer.h"
#include "SetupTriggerPopup.h"
#include "LevelOptionsLayer2.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "InfoAlertButton.h"
#include "GameToolbox.h"
#include "cocos-ext.h"

LevelOptionsLayer* LevelOptionsLayer::create(LevelSettingsObject* object) {
    LevelOptionsLayer* layer = new LevelOptionsLayer();
    if (layer && layer->init(object)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void LevelOptionsLayer::addToggle(const char* title, int tag, bool initialValue, const char* info) {
    cocos2d::CCPoint position = nextPosition(m_page);
    incrementCountForPage(m_page);

    GameToolbox::createToggleButton(
        title,
        menu_selector(LevelOptionsLayer::onToggle),
        tag,
        initialValue,
        m_buttonMenu,
        position,
        this,
        m_mainLayer,
        0.7f,
        m_maxLabelScale,
        110.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt",
        false,
        false,
        objectsForPage(m_page)
    );

    if (info && info[0] != '\0') {
        cocos2d::CCString* infoStr = cocos2d::CCString::create(info);
        m_values->setObject(infoStr, infoKey(tag));
    }
}

void LevelOptionsLayer::didToggle(int tag) {
    switch (tag) {
        case 0:
            m_settingsObject->m_startMini ^= 1;
            break;
        case 1:
            m_settingsObject->m_startDual ^= 1;
            break;
        case 2:
            m_settingsObject->m_twoPlayerMode ^= 1;
            break;
        case 4:
            m_settingsObject->m_mirrorMode ^= 1;
            break;
        case 5:
            m_settingsObject->m_isFlipped ^= 1;
            break;
        case 6:
            m_settingsObject->m_rotateGameplay ^= 1;
            break;
        case 7:
            m_settingsObject->m_reverseGameplay ^= 1;
            break;
        case 8:
            m_settingsObject->m_allowMultiRotation ^= 1;
            break;
        case 9:
            m_settingsObject->m_enablePlayerSqueeze ^= 1;
            break;
        case 10:
            m_settingsObject->m_fixGravityBug ^= 1;
            break;
        case 11:
            m_settingsObject->m_fixNegativeScale ^= 1;
            break;
        case 12:
            m_settingsObject->m_fixRobotJump ^= 1;
            break;
        case 14:
            m_settingsObject->m_dynamicLevelHeight ^= 1;
            break;
        case 15:
            m_settingsObject->m_sortGroups ^= 1;
            break;
        case 16:
            m_settingsObject->m_fixRadiusCollision ^= 1;
            break;
        case 17:
            m_settingsObject->m_enable22Changes ^= 1;
            break;
        case 18:
            m_settingsObject->m_allowStaticRotate ^= 1;
            break;
        case 19:
            m_settingsObject->m_reverseSync ^= 1;
            break;
        case 20:
            m_settingsObject->m_noTimePenalty ^= 1;
            break;
        case 22:
            m_settingsObject->m_decreaseBoostSlide ^= 1;
            break;
        case 23:
            m_settingsObject->m_enableImpulseFix ^= 1;
            break;
        default:
            return;
    }
}

float LevelOptionsLayer::getValue(int tag) {
    if (tag == 13) {
        return static_cast<float>(m_settingsObject->m_spawnGroup);
    }
    return 0.0f;
}

bool LevelOptionsLayer::init(LevelSettingsObject* object) {
    m_settingsObject = object;
    m_gap = 40.0f;
    m_maxLabelScale = 0.4f;

    bool result = GJOptionsLayer::init(1);
    if (result) {
        preSetup();
        postSetup();
    }
    return result;
}

void LevelOptionsLayer::onSettings(cocos2d::CCObject* sender) {
    LevelOptionsLayer2* layer = LevelOptionsLayer2::create(m_settingsObject);
    layer->show();
    onClose(nullptr);
}

void LevelOptionsLayer::setupOptions() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addToggle("Mini Mode", 0, m_settingsObject->m_startMini, "");
    addToggle("Dual Mode", 1, m_settingsObject->m_startDual, "");

    if (!m_settingsObject->m_startsWithStartPos) {
        addToggle("2-Player Mode", 2, m_settingsObject->m_twoPlayerMode,
            "Players are controlled separately.\n"
            "<cl>Player 1:</c> Left side\n"
            "<cg>Player 2:</c> Right side\n"
            "<cy>(You can change the controls in the options menu)</c>");
    }

    addToggle("Mirror Mode", 4, m_settingsObject->m_mirrorMode, "");
    addToggle("Flip\nGravity", 5, m_settingsObject->m_isFlipped, "");
    addToggle("Rotate\nGameplay", 6, m_settingsObject->m_rotateGameplay, "");
    addToggle("Reverse\nGameplay", 7, m_settingsObject->m_reverseGameplay, "");

    if (!m_settingsObject->m_startsWithStartPos) {
        addToggle("No Time\nPenalty", 20, m_settingsObject->m_noTimePenalty,
            "In platformer mode you automatically lose 1000 points per second. Toggle to disable this penalty.");

        cocos2d::CCPoint labelAnchor(0.7f, 0.0f);

        createValueControlAdvanced(
            13,
            "Spawn Group:",
            labelAnchor,
            9999.0f,
            false,
            InputValueType::Float,
            3,
            6,
            0.0f,
            0.0f,
            true,
            false,
            GJInputStyle::GoldLabel,
            0,
            2
        );

        InfoAlertButton* infoBtn = InfoAlertButton::create(
            "Spawn Group",
            "Lets you set a target position for player spawn.\n"
            "If the group contains multiple objects a random target will be selected.",
            0.5f
        );
        m_buttonMenu->addChild(infoBtn);

        cocos2d::CCPoint infoWorld(
            winSize.width * 0.5f + 50.0f - 55.0f,
            winSize.height * 0.5f - 100.0f + 24.0f
        );
        infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(infoWorld));

        cocos2d::CCSprite* settingsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        settingsSprite->setScale(0.6f);
        CCMenuItemSpriteExtra* settingsBtn = CCMenuItemSpriteExtra::create(
            settingsSprite, nullptr, this,
            menu_selector(LevelOptionsLayer::onSettings)
        );
        m_buttonMenu->addChild(settingsBtn);

        cocos2d::CCPoint settingsWorld(
            m_width * 0.5f + winSize.width * 0.5f - 25.0f,
            m_height * 0.5f + winSize.height * 0.5f - 25.0f
        );
        settingsBtn->setPosition(m_buttonMenu->convertToNodeSpace(settingsWorld));
    }
}

void LevelOptionsLayer::valueDidChange(int tag, float value) {
    if (tag == 13) {
        m_settingsObject->m_spawnGroup = static_cast<int>(value);
    }
}

LevelOptionsLayer::~LevelOptionsLayer() {
}