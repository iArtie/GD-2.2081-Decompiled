#pragma once

class GJStoreItem;

class GJPurchaseDelegate {
public:
    virtual void didPurchaseItem(GJStoreItem* item);
    virtual ~GJPurchaseDelegate() = default;
};