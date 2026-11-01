#pragma once

#include "Platform/cocos2d.h"
#include "Cells/CustomSongCell.h"
#include "Delegates/CustomSongDelegate.h"
#include "Models/SongInfoObject.h"

class CustomMusicCell : public CustomSongCell, public CustomSongDelegate {
public:
    static CustomMusicCell* create(const char* identifier, float width, float height);
    bool init();
    
    void loadFromObject(SongInfoObject* object) override;
    void updateBGColor(int index) override;
    
    virtual ~CustomMusicCell();

protected:
    SongInfoObject* m_songInfoObject;
    bool m_selected;
};