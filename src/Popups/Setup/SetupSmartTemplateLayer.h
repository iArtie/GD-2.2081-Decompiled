#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/FLAlertLayerProtocol.h"

class GJSmartTemplate;
class CCTextInputNode;

class SetupSmartTemplateLayer : public FLAlertLayer, public TextInputDelegate, public FLAlertLayerProtocol {
public:
    static SetupSmartTemplateLayer* create(GJSmartTemplate* smartTemplate);
    bool init(GJSmartTemplate* smartTemplate);

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void keyBackClicked() override;
    void onBack(cocos2d::CCObject* sender);
    void onClick(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;

    ~SetupSmartTemplateLayer();

protected:
    GJSmartTemplate* m_smartTemplate = nullptr;
    CCTextInputNode* m_nameInput = nullptr;
    cocos2d::CCNode* m_unk298 = nullptr;
};