#include "Models/LevelSettingsObject.h"
#include "Managers/GJEffectManager.h"
#include "Objects/ColorAction.h"
#include "Utils/GameToolbox.h"
#include "Platform/cocos2d.h"
#include <string>

LevelSettingsObject* LevelSettingsObject::create() {
    LevelSettingsObject* obj = new LevelSettingsObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

std::string LevelSettingsObject::getSaveString() {
    std::string result;

    if (!m_startsWithStartPos) {
        result += "kS38,";
        result += ",,";
        result += m_effectManager->getSaveString();
        result += ",,";
        result += "kA13,";
        result += ",,";
        result += std::to_string(m_songOffset);
        result += ",,";
        result += "kA15,";
        result += ",,";
        result += std::to_string(m_fadeIn);
        result += ",,";
        result += "kA16,";
        result += ",,";
        result += std::to_string(m_fadeOut);
        result += ",,";
        result += "kA14,";
        result += ",,";
        result += m_guidelineString;
        result += ",,";
        result += "kA6,";
        result += ",,";
        result += std::to_string(m_backgroundIndex);
        result += ",,";
        result += "kA7,";
        result += ",,";
        result += std::to_string(m_groundIndex);
        result += ",,";
        result += "kA25,";
        result += ",,";
        result += std::to_string(m_middleGroundIndex);
        result += ",,";
        result += "kA17,";
        result += ",,";
        result += std::to_string(m_groundLineIndex);
        result += ",,";
        result += "kA18,";
        result += ",,";
        result += std::to_string(m_fontIndex);
        result += ",,";
        result += "kS39,";
        result += ",,";
        result += std::to_string(m_colorPage);
        result += ",,";
    }

    result += "kA2,";
    result += std::to_string(m_startMode);
    result += ",";
    result += "kA3,";
    result += std::to_string(m_startMini);
    result += ",";
    result += "kA8,";
    result += std::to_string(m_startDual);
    result += ",";
    result += "kA4,";
    result += std::to_string(m_startSpeed);
    result += ",,";
    result += "kA9,";
    result += std::to_string(m_startsWithStartPos);
    result += ",,";
    result += "kA10,";
    result += ",,";
    result += std::to_string(m_twoPlayerMode);
    result += ",,";
    result += "kA22";
    result += ",,";
    result += std::to_string(m_platformerMode);
    result += ",,";
    result += "kA23,";
    result += ",,";
    result += std::to_string(m_propertykA23);
    result += ",,";
    result += "kA24,";
    result += ",,";
    result += std::to_string(m_propertykA24);
    result += ",,";
    result += "kA27,";
    result += ",,";
    result += std::to_string(m_allowMultiRotation);
    result += ",,";
    result += "kA40,";
    result += ",,";
    result += std::to_string(m_enable22Changes);
    result += ",,";
    result += "kA48";
    result += ",";
    result += std::to_string(m_enableImpulseFix);
    result += ",,";
    result += "kA41";
    result += ",";
    result += std::to_string(m_allowStaticRotate);
    result += ",";
    result += "kA42";
    result += ",";
    result += std::to_string(m_reverseSync);
    result += ",";
    result += "kA28";
    result += ",,";
    result += std::to_string(m_mirrorMode);
    result += ",";
    result += "kA29";
    result += ",,";
    result += std::to_string(m_rotateGameplay);
    result += ",,";
    result += "kA31,";
    result += ",,";
    result += std::to_string(m_enablePlayerSqueeze);
    result += ",";
    result += "kA32,";
    result += ",,";
    result += std::to_string(m_fixGravityBug);
    result += ",";
    result += "kA36,";
    result += ",,";
    result += std::to_string(m_spawnGroup);
    result += ",,";
    result += "kA43,";
    result += ",,";
    result += std::to_string(m_noTimePenalty);
    result += ",";
    result += "kA44,";
    result += ",,";
    result += std::to_string(m_propertykA44);
    result += ",,";
    result += "kA45,";
    result += ",,";
    result += std::to_string(m_decreaseBoostSlide);
    result += ",,";
    result += "kA46";
    result += ",";
    result += std::to_string(m_dontReset);
    result += ",,";
    result += "kA47";
    result += ",";
    result += std::to_string(m_nextFreeID);
    result += ",";
    result += "kA33,";
    result += ",,";
    result += std::to_string(m_fixNegativeScale);
    result += ",,";
    result += "kA34";
    result += ",";
    result += std::to_string(m_fixRobotJump);
    result += ",";
    result += "kA35";
    result += ",,";
    result += std::to_string(m_resetCamera);
    result += ",";
    result += "kA37";
    result += ",,";
    result += std::to_string(m_dynamicLevelHeight);
    result += ",,";
    result += "kA38";
    result += ",,";
    result += std::to_string(m_sortGroups);
    result += ",,";
    result += "kA39";
    result += ",";
    result += std::to_string(m_fixRadiusCollision);
    result += ",";
    result += "kA19";
    result += ",,";
    result += std::to_string(m_targetOrder);
    result += ",";
    result += "kA26";
    result += ",,";
    result += std::to_string(m_targetChannel);
    result += ",,";
    result += "kA20";
    result += ",,";
    result += std::to_string(m_reverseGameplay);
    result += ",,";
    result += "kA21";
    result += ",";
    result += std::to_string(m_disableStartPos);
    result += ",,";
    result += "kA11";
    result += ",";
    result += std::to_string(m_isFlipped);

    return result;
}

bool LevelSettingsObject::init() {
    if (!cocos2d::CCNode::init()) {
        return false;
    }

    m_effectManager = GJEffectManager::create();
    m_effectManager->retain();

    ColorAction* colorAction;

    colorAction = ColorAction::create();
    colorAction->m_fromColor.r = 40;
    colorAction->m_fromColor.g = 125;
    colorAction->m_fromColor.b = 255;
    m_effectManager->setColorAction(colorAction, 1000);

    colorAction = ColorAction::create();
    colorAction->m_fromColor.r = 0;
    colorAction->m_fromColor.g = 102;
    colorAction->m_fromColor.b = 255;
    m_effectManager->setColorAction(colorAction, 1001);

    colorAction = ColorAction::create();
    colorAction->m_fromColor.r = 0;
    colorAction->m_fromColor.g = 102;
    colorAction->m_fromColor.b = 255;
    m_effectManager->setColorAction(colorAction, 1009);

    colorAction = ColorAction::create();
    colorAction->m_fromColor.r = 255;
    colorAction->m_fromColor.g = 255;
    colorAction->m_fromColor.b = 255;
    colorAction->m_blending = true;
    m_effectManager->setColorAction(colorAction, 1002);

    colorAction = ColorAction::create();
    colorAction->m_fromColor.r = 40;
    colorAction->m_fromColor.g = 125;
    colorAction->m_fromColor.b = 255;
    m_effectManager->setColorAction(colorAction, 1013);

    colorAction = ColorAction::create();
    colorAction->m_fromColor.r = 40;
    colorAction->m_fromColor.g = 125;
    colorAction->m_fromColor.b = 255;
    m_effectManager->setColorAction(colorAction, 1014);

    m_startMini = false;
    m_startMode = 0;
    m_startSpeed = 0;
    m_mirrorMode = false;
    m_rotateGameplay = false;
    m_backgroundIndex = 0;
    m_groundIndex = 0;
    m_middleGroundIndex = 0;
    m_startsWithStartPos = false;
    m_allowMultiRotation = true;
    m_enable22Changes = true;
    m_enableImpulseFix = true;
    m_allowStaticRotate = true;
    m_enablePlayerSqueeze = true;
    m_fixGravityBug = true;
    m_fixNegativeScale = true;
    m_fixRobotJump = true;
    m_dynamicLevelHeight = true;
    m_sortGroups = true;
    m_fixRadiusCollision = true;
    m_reverseSync = true;
    m_decreaseBoostSlide = true;
    m_dontReset = false;

    return true;
}

LevelSettingsObject* LevelSettingsObject::objectFromDict(cocos2d::CCDictionary* dict) {
    LevelSettingsObject* obj = LevelSettingsObject::create();
    if (!dict) {
        return obj;
    }

    obj->m_songOffset = dict->valueForKey("kA13")->floatValue();
    obj->m_guidelineString = dict->valueForKey("kA14")->getCString();
    obj->m_fadeIn = dict->valueForKey("kA15")->boolValue();
    obj->m_fadeOut = dict->valueForKey("kA16")->boolValue();
    obj->m_startMode = dict->valueForKey("kA2")->intValue();
    obj->m_startMini = dict->valueForKey("kA3")->boolValue();
    obj->m_startDual = dict->valueForKey("kA8")->boolValue();
    obj->m_twoPlayerMode = dict->valueForKey("kA10")->boolValue();
    obj->m_platformerMode = dict->valueForKey("kA22")->boolValue();
    obj->m_isFlipped = dict->valueForKey("kA11")->boolValue();
    obj->m_propertykA23 = dict->valueForKey("kA23")->boolValue();
    obj->m_propertykA24 = dict->valueForKey("kA24")->boolValue();
    obj->m_startSpeed = dict->valueForKey("kA4")->intValue();
    obj->m_backgroundIndex = dict->valueForKey("kA6")->intValue();
    obj->m_middleGroundIndex = dict->valueForKey("kA25")->intValue();
    obj->m_groundIndex = dict->valueForKey("kA7")->intValue();
    obj->m_groundLineIndex = dict->valueForKey("kA17")->intValue();
    obj->m_fontIndex = dict->valueForKey("kA18")->intValue();
    obj->m_startsWithStartPos = dict->valueForKey("kA9")->boolValue();
    obj->m_colorPage = dict->valueForKey("kS39")->intValue();
    obj->m_targetOrder = dict->valueForKey("kA19")->intValue();
    obj->m_targetChannel = dict->valueForKey("kA26")->intValue();
    obj->m_reverseGameplay = dict->valueForKey("kA20")->boolValue();
    obj->m_disableStartPos = dict->valueForKey("kA21")->boolValue();
    obj->m_allowMultiRotation = dict->valueForKey("kA27")->boolValue();
    obj->m_enable22Changes = dict->valueForKey("kA40")->boolValue();
    obj->m_enableImpulseFix = dict->valueForKey("kA48")->boolValue();
    obj->m_allowStaticRotate = dict->valueForKey("kA41")->boolValue();
    obj->m_reverseSync = dict->valueForKey("kA42")->boolValue();
    obj->m_mirrorMode = dict->valueForKey("kA28")->boolValue();
    obj->m_rotateGameplay = dict->valueForKey("kA29")->boolValue();
    obj->m_enablePlayerSqueeze = dict->valueForKey("kA31")->boolValue();
    obj->m_fixGravityBug = dict->valueForKey("kA32")->boolValue();
    obj->m_fixNegativeScale = dict->valueForKey("kA33")->boolValue();
    obj->m_fixRobotJump = dict->valueForKey("kA34")->boolValue();
    obj->m_dynamicLevelHeight = dict->valueForKey("kA37")->boolValue();
    obj->m_sortGroups = dict->valueForKey("kA38")->boolValue();
    obj->m_fixRadiusCollision = dict->valueForKey("kA39")->boolValue();
    obj->m_resetCamera = dict->valueForKey("kA35")->boolValue();
    obj->m_spawnGroup = dict->valueForKey("kA36")->intValue();
    obj->m_noTimePenalty = dict->valueForKey("kA43")->boolValue();
    obj->m_propertykA44 = dict->valueForKey("kA44")->intValue();
    obj->m_decreaseBoostSlide = dict->valueForKey("kA45")->boolValue();
    obj->m_dontReset = dict->valueForKey("kA46")->boolValue();
    obj->m_nextFreeID = dict->valueForKey("kA47")->intValue();

    if (!obj->m_startsWithStartPos) {
        if (dict->objectForKey("kS38")) {
            obj->m_effectManager->setupFromString(dict->valueForKey("kS38")->getCString());
        } else if (dict->objectForKey("kS29")) {
            ColorAction* ca;

            ca = obj->m_effectManager->getColorAction(1000);
            ca->setupFromString(dict->valueForKey("kS29")->getCString());

            ca = obj->m_effectManager->getColorAction(1001);
            ca->setupFromString(dict->valueForKey("kS30")->getCString());

            ca = obj->m_effectManager->getColorAction(1002);
            ca->setupFromString(dict->valueForKey("kS31")->getCString());
            ca->m_blending = true;

            ca = obj->m_effectManager->getColorAction(1004);
            ca->setupFromString(dict->valueForKey("kS32")->getCString());

            ca = obj->m_effectManager->getColorAction(1);
            ca->setupFromString(dict->valueForKey("kS33")->getCString());

            ca = obj->m_effectManager->getColorAction(2);
            ca->setupFromString(dict->valueForKey("kS34")->getCString());

            ca = obj->m_effectManager->getColorAction(3);
            ca->setupFromString(dict->valueForKey("kS35")->getCString());

            ca = obj->m_effectManager->getColorAction(4);
            ca->setupFromString(dict->valueForKey("kS36")->getCString());

            ca = obj->m_effectManager->getColorAction(1003);
            ca->setupFromString(dict->valueForKey("kS37")->getCString());
        } else if (dict->objectForKey("kS1")) {
            obj->setupColorsFromLegacyMode(dict);
        }
    }

    return obj;
}

LevelSettingsObject* LevelSettingsObject::objectFromString(const std::string& str) {
    cocos2d::CCDictionary* dict = GameToolbox::stringSetupToDict(str, ",");
    return LevelSettingsObject::objectFromDict(dict);
}

void LevelSettingsObject::setupColorsFromLegacyMode(cocos2d::CCDictionary* dict) {
    int bgR = dict->valueForKey("kS1")->intValue();
    int bgG = dict->valueForKey("kS2")->intValue();
    int bgB = dict->valueForKey("kS3")->intValue();

    int gR = dict->valueForKey("kS4")->intValue();
    int gG = dict->valueForKey("kS5")->intValue();
    int gB = dict->valueForKey("kS6")->intValue();

    int obR = -1, obG = -1, obB = -1;
    if (dict->objectForKey("kS7")) {
        obR = dict->valueForKey("kS7")->intValue();
        obG = dict->valueForKey("kS8")->intValue();
        obB = dict->valueForKey("kS9")->intValue();
    }

    int lineR = -1, lineG = -1, lineB = -1;
    if (dict->objectForKey("kS10")) {
        lineR = dict->valueForKey("kS10")->intValue();
        lineG = dict->valueForKey("kS11")->intValue();
        lineB = dict->valueForKey("kS12")->intValue();
    }

    int col1R = -1, col1G = -1, col1B = -1;
    if (dict->objectForKey("kS13")) {
        col1R = dict->valueForKey("kS13")->intValue();
        col1G = dict->valueForKey("kS14")->intValue();
        col1B = dict->valueForKey("kS15")->intValue();
    }

    int col2R = -1, col2G = -1, col2B = -1;
    if (dict->objectForKey("kS21")) {
        col2R = dict->valueForKey("kS21")->intValue();
        col2G = dict->valueForKey("kS22")->intValue();
        col2B = dict->valueForKey("kS23")->intValue();
    }

    int col3R = -1, col3G = -1, col3B = -1;
    if (dict->objectForKey("kS24")) {
        col3R = dict->valueForKey("kS24")->intValue();
        col3G = dict->valueForKey("kS25")->intValue();
        col3B = dict->valueForKey("kS26")->intValue();
    }

    int p1Color = dict->valueForKey("kS16")->intValue();
    int p2Color = dict->valueForKey("kS17")->intValue();
    int objColor = dict->valueForKey("kS18")->intValue();
    int lbgColor = dict->valueForKey("kS19")->intValue();
    int col1Color = dict->valueForKey("kS20")->intValue();
    int col2Color = dict->valueForKey("kS28")->intValue();
    bool objBlend = dict->valueForKey("kA5")->boolValue();
    bool colBlend = dict->valueForKey("kA12")->boolValue();

    ColorAction* ca;

    ca = m_effectManager->getColorAction(1000);
    ca->m_fromColor.r = bgR;
    ca->m_fromColor.g = bgG;
    ca->m_fromColor.b = bgB;
    ca->m_playerColor = p1Color;

    ca = m_effectManager->getColorAction(1001);
    ca->m_fromColor.r = gR;
    ca->m_fromColor.g = gG;
    ca->m_fromColor.b = gB;
    ca->m_playerColor = p2Color;

    ca = m_effectManager->getColorAction(1002);
    ca->m_fromColor.r = lineR;
    ca->m_fromColor.g = lineG;
    ca->m_fromColor.b = lineB;
    ca->m_playerColor = objColor;
    ca->m_blending = true;

    ca = m_effectManager->getColorAction(1004);
    ca->m_playerColor = lbgColor;
    ca->m_fromColor.r = obR;
    ca->m_fromColor.g = obG;
    ca->m_fromColor.b = obB;

    ca = m_effectManager->getColorAction(1);
    ca->m_fromColor.r = col1R;
    ca->m_fromColor.g = col1G;
    ca->m_fromColor.b = col1B;
    ca->m_playerColor = col1Color;
    ca->m_blending = objBlend;

    ca = m_effectManager->getColorAction(2);
    ca->m_fromColor.r = col2R;
    ca->m_fromColor.g = col2G;
    ca->m_fromColor.b = col2B;
    ca->m_playerColor = col2Color;

    ca = m_effectManager->getColorAction(3);
    ca->m_fromColor.r = col3R;
    ca->m_fromColor.g = col3G;
    ca->m_fromColor.b = col3B;
    ca->m_blending = colBlend;

    ca = m_effectManager->getColorAction(4);
    ca->m_fromColor.r = -1;
    ca->m_fromColor.g = -1;
    ca->m_fromColor.b = -1;
}

bool LevelSettingsObject::shouldUseYSection() {
    if (!m_platformerMode) {
        return m_dynamicLevelHeight;
    }
    return m_platformerMode;
}

LevelSettingsObject::~LevelSettingsObject() {
    if (m_effectManager) {
        m_effectManager->release();
    }
}