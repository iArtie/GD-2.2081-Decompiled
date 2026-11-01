#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"
#include <string>

class SelectSettingDelegate;

class SelectSettingLayer : public FLAlertLayer {
public:
    static SelectSettingLayer* create(SelectSettingType type, int value);
    static std::string frameForItem(SelectSettingType type, int index);
    static std::string frameForValue(SelectSettingType type, int value);
    static int idxToValue(SelectSettingType type, int idx);
    static int valueToIdx(SelectSettingType type, int value);

    bool init(SelectSettingType type, int value) override;

    std::string getSelectedFrame();
    int getSelectedValue();

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender) override;
    void onSelect(cocos2d::CCObject* sender);

    ~SelectSettingLayer();

protected:
    SelectSettingDelegate* m_delegate = nullptr;
    int m_settingID = 0;
    cocos2d::CCArray* m_settingSprites = nullptr;
    SelectSettingType m_type = SelectSettingType::StartingMode;
};