#pragma once

#include "cocos2d.h"
#include "GJDropDownLayer.h"

class GJListLayer;

class StatsLayer : public GJDropDownLayer {
public:
    static StatsLayer* create();
    void customSetup() override;
    virtual ~StatsLayer();

protected:
    GJListLayer* m_listLayer = nullptr;
};