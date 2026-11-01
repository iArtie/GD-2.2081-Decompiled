#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"

class LikeItemDelegate;

class LikeItemLayer : public FLAlertLayer {
public:
    static LikeItemLayer* create(LikeItemType type, int id, int parentID);

    bool init(LikeItemType type, int id, int parentID);
    void keyBackClicked() override;

    void onClose(cocos2d::CCObject* sender);
    void onDislike(cocos2d::CCObject* sender);
    void onLike(cocos2d::CCObject* sender);
    void triggerLike(bool isLiked);

    virtual ~LikeItemLayer();

protected:
    int m_itemID = 0;
    int m_commentSourceID = 0;
    LikeItemType m_itemType = LikeItemType::Level;
    LikeItemDelegate* m_likeDelegate = nullptr;
};