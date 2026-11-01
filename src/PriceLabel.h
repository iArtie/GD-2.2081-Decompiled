#pragma once

#include "cocos2d.h"

class PriceLabel : public cocos2d::CCNode {
public:
    static PriceLabel* create(int price);

    bool init(int price);
    void setOpacity(float opacity);
    void setPrice(int price);

    virtual ~PriceLabel();

protected:
    cocos2d::CCLabelBMFont* m_priceLabel = nullptr;
    cocos2d::CCSprite* m_coinSprite = nullptr;
};