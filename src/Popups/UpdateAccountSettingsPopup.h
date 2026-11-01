#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"

class TextArea;
class GJAccountSettingsLayer;
class LoadingCircle;

class UpdateAccountSettingsPopup : public FLAlertLayer {
public:
    bool init(GJAccountSettingsLayer* settingsLayer, int accountID, std::string username, std::string password, std::string email, std::string secret, std::string saveData);

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void updateSettingsFailed();
    void updateSettingsFinished();

    ~UpdateAccountSettingsPopup();

protected:
    LoadingCircle* m_loadingCircle = nullptr;
    TextArea* m_textArea = nullptr;
    bool m_updateSuccess = false;
    GJAccountSettingsLayer* m_settingsLayer = nullptr;
};