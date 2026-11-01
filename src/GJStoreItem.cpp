#include "GJStoreItem.h"

GJStoreItem* GJStoreItem::create(int index, int typeID, int unlockType, int price, ShopType shopType) {
    auto item = new GJStoreItem();
    if (item && item->init(index, typeID, unlockType, price, shopType)) {
        item->autorelease();
        return item;
    }
    delete item;
    return nullptr;
}

bool GJStoreItem::init(int index, int typeID, int unlockType, int price, ShopType shopType) {
    if (!CCNode::init()) {
        return false;
    }
    
    m_index.value = index;
    int randVal = rand();
    m_index.seed = static_cast<int>((randVal * 4.6566e-10f) * 1000.0f);
    m_index.random = index + m_index.seed;
    
    m_typeID.value = typeID;
    randVal = rand();
    m_typeID.seed = static_cast<int>((randVal * 4.6566e-10f) * 1000.0f);
    m_typeID.random = typeID + m_typeID.seed;
    
    m_unlockType.value = unlockType;
    randVal = rand();
    m_unlockType.seed = static_cast<int>((randVal * 4.6566e-10f) * 1000.0f);
    m_unlockType.random = unlockType + m_unlockType.seed;
    
    m_price.value = price;
    randVal = rand();
    m_price.seed = static_cast<int>((randVal * 4.6566e-10f) * 1000.0f);
    m_price.random = price + m_price.seed;
    
    m_shopType = shopType;
    
    return true;
}

std::string GJStoreItem::getCurrencyKey() {
    const char* key = (m_shopType == ShopType::Diamond) ? "29" : "14";
    return std::string(key);
}

GJStoreItem::~GJStoreItem() {
}