#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"

class SelectPremadeDelegate;

class SelectPremadeLayer : public FLAlertLayer {
public:
    static SelectPremadeLayer* create();
    bool init() override;

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender) override;
    void onSelectPremade(cocos2d::CCObject* sender);

    ~SelectPremadeLayer();

protected:
    SelectPremadeDelegate* m_delegate = nullptr;
};