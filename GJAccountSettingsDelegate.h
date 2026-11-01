#pragma once

class GJAccountSettingsDelegate
{
public:
    virtual void updateSettingsFailed();
    virtual void updateSettingsFinished();
    
    virtual ~GJAccountSettingsDelegate() {}
};