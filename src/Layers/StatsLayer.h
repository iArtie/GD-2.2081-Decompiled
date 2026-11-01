#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJDropDownLayer.h"

class GJListLayer;

class StatsLayer : public GJDropDownLayer {
public:
    static StatsLayer* create();
    void customSetup() override;
    virtual ~StatsLayer();

protected:
    GJListLayer* m_listLayer = nullptr;
};