#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "FLAlertLayer.h"

class SelectSFXSortDelegate;

class SelectSFXSortLayer : public FLAlertLayer {
public:
    static SelectSFXSortLayer* create(AudioSortType sortType);
    bool init(AudioSortType sortType) override;

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender) override;
    void onSelect(cocos2d::CCObject* sender);

    ~SelectSFXSortLayer();

protected:
    AudioSortType m_sortType = AudioSortType::NameAscending;
    SelectSFXSortDelegate* m_delegate = nullptr;
};