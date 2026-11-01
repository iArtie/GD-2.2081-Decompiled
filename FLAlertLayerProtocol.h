#pragma once

class FLAlertLayer;

class FLAlertLayerProtocol
{
public:
    virtual void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    virtual ~FLAlertLayerProtocol() {}
};