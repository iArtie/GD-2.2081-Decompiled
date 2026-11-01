#pragma once

#include "cocos2d.h"
#include "SetTextPopup.h"
#include <string>

class SearchSFXPopup : public SetTextPopup {
public:
    static SearchSFXPopup* create(std::string query);

    bool init(std::string query) override;

    void onSearchFolders(cocos2d::CCObject* sender);

    virtual ~SearchSFXPopup();
};