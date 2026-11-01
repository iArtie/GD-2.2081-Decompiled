#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJDropDownLayer.h"
#include <string>

class URLViewLayer : public GJDropDownLayer {
public:
    static URLViewLayer* create(std::string title, cocos2d::CCArray* objects);
    bool init(std::string title, cocos2d::CCArray* objects) override;

    ~URLViewLayer();

protected:
    cocos2d::CCArray* m_urlObjects = nullptr;
    CustomListView* m_listLayer = nullptr;
};