#include "StarInfoPopup.h"

StarInfoPopup* StarInfoPopup::createFromString(std::string str) {
    std::vector<std::string> parts;
    GameToolbox::splitString(str, ",", &parts);

    StarInfoPopup* popup;
    if (parts.size() <= 7) {
        popup = StarInfoPopup::create(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    } else {
        int stars = atoi(parts[0].c_str());
        int easies = atoi(parts[1].c_str());
        int normals = atoi(parts[2].c_str());
        int hards = atoi(parts[3].c_str());
        int harders = atoi(parts[4].c_str());
        int insanes = atoi(parts[5].c_str());
        int dailies = atoi(parts[6].c_str());
        int weeklies = atoi(parts[7].c_str());
        popup = StarInfoPopup::create(stars, easies, normals, hards, harders, insanes, dailies, weeklies, 0, 0);
    }
    return popup;
}

StarInfoPopup* StarInfoPopup::createFromStringMoons(std::string str) {
    std::vector<std::string> parts;
    GameToolbox::splitString(str, ",", &parts);

    StarInfoPopup* popup;
    if (parts.size() <= 6) {
        popup = StarInfoPopup::create(0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    } else {
        int moons = atoi(parts[0].c_str());
        int easies = atoi(parts[1].c_str());
        int normals = atoi(parts[2].c_str());
        int hards = atoi(parts[3].c_str());
        int harders = atoi(parts[4].c_str());
        int insanes = atoi(parts[5].c_str());
        int dailies = atoi(parts[6].c_str());
        popup = StarInfoPopup::create(moons, easies, normals, hards, harders, insanes, 0, 0, dailies, 1);
    }
    return popup;
}

void StarInfoPopup::keyBackClicked() {
    onClose(nullptr);
}

void StarInfoPopup::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

StarInfoPopup::~StarInfoPopup() {
}