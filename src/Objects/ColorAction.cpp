#include "Objects/ColorAction.h"
#include "Utils/GameToolbox.h"
#include "Platform/cocos2d.h"

ColorAction* ColorAction::create() {
    ColorAction* action = new ColorAction();
    if (action) {
        action->autorelease();
        action->resetAction();
    }
    return action;
}

ColorAction* ColorAction::getCopy() {
    ColorAction* copy = new ColorAction();
    if (copy) {
        copy->autorelease();
        copy->m_fromColor = m_fromColor;
        copy->m_toColor = m_toColor;
        copy->m_color = m_color;
        copy->m_duration = m_duration;
        copy->m_fromOpacity = m_fromOpacity;
        copy->m_playerColor = m_playerColor;
        copy->m_toOpacity = m_toOpacity;
        copy->m_colorID = m_colorID;
        copy->m_copyHSV = m_copyHSV;
        copy->m_blending = m_blending;
        copy->m_copyOpacity = m_copyOpacity;
        copy->m_copyID = m_copyID;
        copy->m_deltaTime = m_deltaTime;
        copy->m_paused = m_paused;
        copy->m_uniqueID = m_uniqueID;
        copy->m_currentOpacity = m_currentOpacity;
        copy->m_legacyHSV = m_legacyHSV;
        copy->resetAction();
    }
    return copy;
}

bool ColorAction::isInUse() {
    // Check if from color is not default (255,255,255)
    if (m_fromColor.r != 255 || m_fromColor.g != 255 || m_fromColor.b != 255) {
        return true;
    }
    // Check other properties that indicate usage
    if (m_fromOpacity != 1.0f || m_copyID != 0 || m_blending) {
        return true;
    }
    return m_playerColor > 0;
}

void ColorAction::loadFromState(CAState* state) {
    m_fromColor = state->m_fromColor;
    m_toColor = state->m_toColor;
    m_color = state->m_color;
    m_duration = state->m_duration;
    m_fromOpacity = state->m_fromOpacity;
    m_playerColor = state->m_playerColor;
    m_toOpacity = state->m_toOpacity;
    m_colorID = state->m_colorID;
    m_copyHSV = state->m_copyHSV;
    m_blending = state->m_blending;
    m_copyOpacity = state->m_copyOpacity;
    m_copyID = state->m_copyID;
    m_deltaTime = state->m_deltaTime;
    m_paused = state->m_paused;
    m_uniqueID = state->m_uniqueID;
    m_currentOpacity = state->m_currentOpacity;
    m_legacyHSV = state->m_legacyHSV;
}

void ColorAction::resetAction() {
    m_stepFinished = false;
    m_paused = false;
    m_deltaTime = 0.0f;
}

void ColorAction::saveToState(CAState* state) {
    state->m_fromColor = m_fromColor;
    state->m_toColor = m_toColor;
    state->m_color = m_color;
    state->m_paused = m_paused;
    state->m_playerColor = m_playerColor;
    state->m_colorID = m_colorID;
    state->m_blending = m_blending;
    state->m_copyOpacity = m_copyOpacity;
    state->m_copyID = m_copyID;
    state->m_duration = m_duration;
    state->m_fromOpacity = m_fromOpacity;
    state->m_toOpacity = m_toOpacity;
    state->m_copyHSV = m_copyHSV;
    state->m_deltaTime = m_deltaTime;
    state->m_legacyHSV = m_legacyHSV;
    state->m_uniqueID = m_uniqueID;
    state->m_currentOpacity = m_currentOpacity;
}

void ColorAction::setupFromMap(std::map<std::string, std::string>* setup) {
    // Parse from color (keys: 1, 2, 3)
    if (auto it = setup->find("1"); it != setup->end()) {
        m_fromColor.r = atoi(it->second.c_str());
    }
    if (auto it = setup->find("2"); it != setup->end()) {
        m_fromColor.g = atoi(it->second.c_str());
    }
    if (auto it = setup->find("3"); it != setup->end()) {
        m_fromColor.b = atoi(it->second.c_str());
    }

    // Parse to color (keys: 11, 12, 13)
    if (auto it = setup->find("11"); it != setup->end()) {
        m_toColor.r = atoi(it->second.c_str());
    }
    if (auto it = setup->find("12"); it != setup->end()) {
        m_toColor.g = atoi(it->second.c_str());
    }
    if (auto it = setup->find("13"); it != setup->end()) {
        m_toColor.b = atoi(it->second.c_str());
    }

    // Player color (key: 4)
    if (auto it = setup->find("4"); it != setup->end()) {
        m_playerColor = atoi(it->second.c_str());
    }

    // Duration (key: 16)
    if (auto it = setup->find("16"); it != setup->end()) {
        m_duration = atof(it->second.c_str());
    }

    // Color ID (key: 6)
    if (auto it = setup->find("6"); it != setup->end()) {
        m_colorID = atoi(it->second.c_str());
    }

    // Blending (key: 5)
    if (auto it = setup->find("5"); it != setup->end()) {
        m_blending = atof(it->second.c_str()) != 0.0f;
    }

    // Copy opacity (key: 17)
    if (auto it = setup->find("17"); it != setup->end()) {
        m_copyOpacity = atoi(it->second.c_str()) != 0;
    }

    // From opacity (key: 8, 7)
    m_fromOpacity = 1.0f;
    if (auto it = setup->find("8"); it != setup->end()) {
        int hasFromOpacity = atoi(it->second.c_str());
        if (hasFromOpacity > 0) {
            if (auto it2 = setup->find("7"); it2 != setup->end()) {
                m_fromOpacity = atof(it2->second.c_str());
            }
        }
    }

    // To opacity (key: 15)
    if (auto it = setup->find("15"); it != setup->end()) {
        m_toOpacity = atof(it->second.c_str());
    }

    // Copy ID (key: 9)
    if (auto it = setup->find("9"); it != setup->end()) {
        m_copyID = atoi(it->second.c_str());
    }

    // Copy HSV (key: 10)
    if (auto it = setup->find("10"); it != setup->end()) {
        m_copyHSV = GameToolbox::hsvFromString(it->second, &byte_F7A380);
    }

    // Delta time (key: 14)
    if (auto it = setup->find("14"); it != setup->end()) {
        m_deltaTime = atof(it->second.c_str());
    }

    // Paused (key: 19)
    if (auto it = setup->find("19"); it != setup->end()) {
        m_paused = atoi(it->second.c_str()) != 0;
    }
}

void ColorAction::setupFromString(std::string setup) {
    std::map<std::string, std::string> parsedMap;
    GameToolbox::stringSetupToMap(setup, "_", &parsedMap);
    setupFromMap(&parsedMap);

    // Clean up map
    for (auto& pair : parsedMap) {
        // Strings will be freed by map destructor
    }
}

void ColorAction::step(float dt) {
    if (m_stepFinished || m_paused) {
        return;
    }

    float newTime = dt + m_deltaTime;
    float intervalTime = newTime + m_stepInterval;
    m_deltaTime = newTime;

    if (intervalTime < m_duration) {
        if (intervalTime <= 0.0f) {
            m_color = m_fromColor;
            m_currentOpacity = m_fromOpacity;
        } else {
            float progress = intervalTime / m_duration;
            if (progress < 1.0f && progress > 0.0f) {
                m_color = GameToolbox::multipliedColorValue(
                    m_fromColor,
                    m_toColor,
                    progress
                );
                m_currentOpacity = m_fromOpacity + (progress * (m_toOpacity - m_fromOpacity));
            } else {
                m_color = m_fromColor;
                m_currentOpacity = m_fromOpacity;
            }
        }
        m_stepFinished = (intervalTime >= m_duration);
    } else {
        m_color = m_toColor;
        m_currentOpacity = m_toOpacity;
        m_stepFinished = true;
    }
}

ColorAction::~ColorAction() {
    // Destructor
}