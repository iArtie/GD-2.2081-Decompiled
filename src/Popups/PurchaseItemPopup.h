#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"

class GJStoreItem;
class GJPurchaseDelegate;

class PurchaseItemPopup : public FLAlertLayer {
public:
    static PurchaseItemPopup* create(GJStoreItem* item);

    bool init(GJStoreItem* item);
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onPurchase(cocos2d::CCObject* sender);

    virtual ~PurchaseItemPopup();

protected:
    GJStoreItem* m_storeItem = nullptr;
    GJPurchaseDelegate* m_delegate = nullptr;
};