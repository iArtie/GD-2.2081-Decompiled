#include "Models/GJRewardObject.h"
#include "Utils/DS_Dictionary.h"

GJRewardObject* GJRewardObject::create() {
    auto obj = new GJRewardObject();
    if (obj && obj->init(SpecialRewardItem::FireShard, 0, 0)) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

GJRewardObject* GJRewardObject::createItemUnlock(UnlockType type, int id) {
    auto obj = GJRewardObject::create();
    obj->m_unlockType = type;
    return obj;
}

GJRewardObject* GJRewardObject::createWithCoder(DS_Dictionary* dict) {
    auto obj = GJRewardObject::create();
    obj->dataLoaded(dict);
    return obj;
}

bool GJRewardObject::canEncode() {
    return true;
}

void GJRewardObject::dataLoaded(DS_Dictionary* dict) {
    m_specialRewardItem = static_cast<SpecialRewardItem>(dict->getIntegerForKey("1"));
    m_itemID = dict->getIntegerForKey("2");
    m_total = dict->getIntegerForKey("3");
    m_unlockType = static_cast<UnlockType>(dict->getIntegerForKey("4"));
}

void GJRewardObject::encodeWithCoder(DS_Dictionary* dict) {
    dict->setIntegerForKey("kCEK", 9);
    dict->setIntegerForKey("1", static_cast<int>(m_specialRewardItem));
    dict->setIntegerForKey("2", m_itemID);
    dict->setIntegerForKey("3", m_total);
    dict->setIntegerForKey("4", static_cast<int>(m_unlockType));
}

bool GJRewardObject::init(SpecialRewardItem specialRewardItem, int total, int itemID) {
    m_specialRewardItem = specialRewardItem;
    m_itemID = itemID;
    m_total = total;
    m_unlockType = UnlockType::Unknown;
    return true;
}

bool GJRewardObject::isSpecialType() {
    switch (m_specialRewardItem) {
        case SpecialRewardItem::FireShard:
        case SpecialRewardItem::IceShard:
        case SpecialRewardItem::PoisonShard:
        case SpecialRewardItem::ShadowShard:
        case SpecialRewardItem::LavaShard:
        case SpecialRewardItem::BonusKey:
        case SpecialRewardItem::EarthShard:
        case SpecialRewardItem::BloodShard:
        case SpecialRewardItem::MetalShard:
        case SpecialRewardItem::LightShard:
        case SpecialRewardItem::SoulShard:
        case SpecialRewardItem::GoldKey:
            return true;
        default:
            return false;
    }
}

GJRewardObject::~GJRewardObject() {
}