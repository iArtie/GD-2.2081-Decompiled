#pragma once

#include "cocos2d.h"
#include <string>

struct GJGameValue {
    int value;
    int seed;
    int random;
};

enum class ShopType {
    Main = 0,
    Secret = 1,
    Community = 2,
    Mechanic = 3,
    Diamond = 4
};

class GJStoreItem : public cocos2d::CCNode {
public:
    static GJStoreItem* create(int index, int typeID, int unlockType, int price, ShopType shopType);
    std::string getCurrencyKey();
    bool init(int index, int typeID, int unlockType, int price, ShopType shopType) override;
    virtual ~GJStoreItem();

    GJGameValue m_index;
    GJGameValue m_typeID;
    GJGameValue m_unlockType;
    GJGameValue m_price;
    ShopType m_shopType;
};