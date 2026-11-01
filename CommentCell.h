#pragma once

#include "cocos2d.h"
#include "TableViewCell.h"
#include "LikeItemDelegate.h"
#include "FLAlertLayerProtocol.h"

class GJComment;
class GJUserScore;
class SimplePlayer;
class TextArea;

class CommentCell : public TableViewCell, public LikeItemDelegate, public FLAlertLayerProtocol {
public:
    CommentCell(char* charPtr, float value, float value2);

    virtual void draw() override;
    virtual void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    virtual void likedItem(LikeItemType type, int id, bool liked) override;

    bool init();
    void incrementDislikes();
    void incrementLikes();
    void loadFromComment(GJComment* comment);
    void onConfirmDelete(cocos2d::CCObject* sender);
    void onDelete();
    void onGoToLevel(cocos2d::CCObject* sender);
    void onLike(cocos2d::CCObject* sender);
    void onUndelete();
    void onUnhide(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    void updateLabelValues();

    ~CommentCell();

private:
    cocos2d::CCSprite* m_iconSprite;
    cocos2d::CCLabelBMFont* m_likeLabel;
    GJComment* m_comment;
    bool m_compactMode;
    float m_width;
    float m_height;
    bool m_accountComment;
    cocos2d::CCLayerColor* m_backgroundLayer;
    cocos2d::CCLayer* m_mainLayer;
};