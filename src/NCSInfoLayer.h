#pragma once

#include "cocos2d.h"
#include "GJEnums.h"

class CustomSongLayer;
class FLAlertLayerProtocol;

class NCSInfoLayer : public FLAlertLayer {
public:
    static NCSInfoLayer* create(CustomSongLayer* layer);

    bool init(CustomSongLayer* layer);

    virtual void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onLibrary(cocos2d::CCObject* sender);
    void onNCS(cocos2d::CCObject* sender);
    void onNCSIO(cocos2d::CCObject* sender);
    void onNCSUsage(cocos2d::CCObject* sender);

    virtual ~NCSInfoLayer();

protected:
    CustomSongLayer* m_songLayer = nullptr;
};