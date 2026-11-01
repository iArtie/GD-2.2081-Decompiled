#pragma once

#include "cocos2d.h"
#include "BoomListView.h"

enum BoomListType {
    User = 2,
    User2 = 3,
    Stats = 4,
    Achievement = 5,
    Level = 6,
    Level2 = 7,
    Comment = 8,
    Comment2 = 9,
    Comment3 = 10,
    Comment4 = 11,
    Song = 12,
    Score = 13,
    MapPack = 14,
    CustomSong = 15,
    Request = 16,
    Message = 17,
    LevelScore = 18,
    Artist = 19,
    SmartTemplate = 20,
    SFX = 21,
    SFX2 = 22,
    CustomMusic = 23,
    Options = 24,
    LevelList = 25,
    LevelList2 = 26,
    Level3 = 27,
    Level4 = 28,
    LevelList3 = 29,
    LocalLevelScore = 30,
    URL = 31,
    List = 32
};

class CustomListView : public BoomListView {
public:
    static CustomListView* create(cocos2d::CCArray* entries, BoomListType type, float height, float width);
    static float getCellHeight(BoomListType type);
    
    TableViewCell* getListCell(const char* identifier) override;
    void loadCell(TableViewCell* cell, int index) override;
    void reloadAll() override;
    void setupList(float yOffset) override;
    
    virtual ~CustomListView();

protected:
    BoomListType m_type;
    cocos2d::CCArray* m_entries;
    float m_itemSeparation;
    float m_height;
    TableView* m_tableView;
    bool m_locked;
    int m_cellMode;
    float m_width;
};