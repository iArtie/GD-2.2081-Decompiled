#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/TableViewCellDelegate.h"

class GJCommentListLayer;

class OptionsScrollLayer : public FLAlertLayer, public TableViewCellDelegate {
public:
    static OptionsScrollLayer* create(cocos2d::CCArray* objects, bool recreate, int minimum);

    bool init(cocos2d::CCArray* objects, bool recreate, int minimum);

    cocos2d::CCArray* getRelevantObjects(cocos2d::CCArray* objects);
    void setupList(cocos2d::CCArray* objects);

    virtual void keyBackClicked() override;
    virtual void registerWithTouchDispatcher() override;

    void onClose(cocos2d::CCObject* sender);

    virtual ~OptionsScrollLayer();

protected:
    GJCommentListLayer* m_listLayer = nullptr;
    FLAlertLayerProtocol* m_alertProtocol = nullptr;
    int m_minCount = 0;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    bool m_recreateList = false;
    cocos2d::CCArray* m_optionObjects = nullptr;
    cocos2d::CCMenu* m_buttonMenu = nullptr;
};