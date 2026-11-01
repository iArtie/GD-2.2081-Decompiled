#pragma once

#include "cocos2d.h"
#include "GJDropDownLayer.h"

class SongsLayer : public GJDropDownLayer {
public:
    static SongsLayer* create();
    void customSetup() override;
    virtual ~SongsLayer();

protected:
    cocos2d::CCLayer* m_listLayer = nullptr;
};