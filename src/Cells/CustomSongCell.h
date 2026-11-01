#pragma once

#include "Platform/cocos2d.h"
#include "Cells/TableViewCell.h"
#include "Delegates/CustomSongDelegate.h"
#include "Models/SongInfoObject.h"
#include <string>

class CustomSongCell : public TableViewCell, public CustomSongDelegate {
public:
    static CustomSongCell* create(const char* identifier, float width, float height);
    bool init();
    
    void draw() override;
    void loadFromObject(SongInfoObject* object);
    void onDelete(cocos2d::CCObject* sender);
    void updateBGColor(int index) override;
    bool shouldReload();
    int getActiveSongID();
    void songIDChanged(int id) override;
    
    // CustomSongDelegate methods
    LevelSettingsObject* getLevelSettings() override;
    std::string getSongFileName() override;
    
    virtual ~CustomSongCell();

protected:
    SongInfoObject* m_songInfoObject;
    bool m_selected;
};