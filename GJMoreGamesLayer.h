#pragma once

#include "cocos2d.h"

class GJMoreGamesLayer : public GJDropDownLayer {
public:
    static GJMoreGamesLayer* create();
    void customSetup() override;
    cocos2d::CCArray* getMoreGamesList();
    virtual ~GJMoreGamesLayer();

private:
    cocos2d::CCArray* m_moreGamesList;
};