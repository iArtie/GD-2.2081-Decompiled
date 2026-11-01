#include "StatsLayer.h"
#include "GameStatsManager.h"
#include "StatsObject.h"
#include "CustomListView.h"

StatsLayer* StatsLayer::create() {
    StatsLayer* returnVal = new StatsLayer();
    if (GJDropDownLayer::init("Stats")) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void StatsLayer::customSetup() {
    cocos2d::CCArray* objects = cocos2d::CCArray::create();

    objects->addObject(StatsObject::create("1", GameStatsManager::sharedState()->getStat("1")));
    objects->addObject(StatsObject::create("2", GameStatsManager::sharedState()->getStat("2")));
    objects->addObject(StatsObject::create("6", GameStatsManager::sharedState()->getStat("6")));
    objects->addObject(StatsObject::create("13", GameStatsManager::sharedState()->getStat("13")));
    objects->addObject(StatsObject::create("14", GameStatsManager::sharedState()->getStat("22")));
    objects->addObject(StatsObject::create("3", GameStatsManager::sharedState()->getStat("3")));
    objects->addObject(StatsObject::create("4", GameStatsManager::sharedState()->getStat("4")));
    objects->addObject(StatsObject::create("42", GameStatsManager::sharedState()->getStat("42")));
    objects->addObject(StatsObject::create("5", GameStatsManager::sharedState()->getStat("5")));
    objects->addObject(StatsObject::create("15", GameStatsManager::sharedState()->getStat("15")));
    objects->addObject(StatsObject::create("8", GameStatsManager::sharedState()->getStat("8")));
    objects->addObject(StatsObject::create("12", GameStatsManager::sharedState()->getStat("12")));
    objects->addObject(StatsObject::create("7", GameStatsManager::sharedState()->getStat("7")));
    objects->addObject(StatsObject::create("40", GameStatsManager::sharedState()->getStat("40")));
    objects->addObject(StatsObject::create("10", GameStatsManager::sharedState()->getStat("10")));
    objects->addObject(StatsObject::create("11", GameStatsManager::sharedState()->getStat("11")));
    objects->addObject(StatsObject::create("41", GameStatsManager::sharedState()->getStat("41")));

    if (GameStatsManager::sharedState()->getStat("9") > 0) {
        objects->addObject(StatsObject::create("9", GameStatsManager::sharedState()->getStat("9")));
    }

    CustomListView* listView = CustomListView::create(objects, BoomListType::Stats, 220.0f, 356.0f);
    m_listLayer->addChild(listView, 6);
    m_listLayer->setColor(cocos2d::ccc3(191, 114, 62));
    m_listLayer->setOpacity(255);
}

StatsLayer::~StatsLayer() {
}