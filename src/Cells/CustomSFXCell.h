#pragma once

#include "Platform/cocos2d.h"
#include "Cells/TableViewCell.h"
#include "Delegates/CustomSFXDelegate.h"
#include "Models/SFXInfoObject.h"

class CustomSFXCell : public TableViewCell, public CustomSFXDelegate {
public:
    static CustomSFXCell* create(const char* identifier, float width, float height);
    bool init();
    
    void draw() override;
    void loadFromObject(SFXInfoObject* object);
    void sfxObjectSelected(SFXInfoObject* object) override;
    void updateBGColor(int index) override;
    bool shouldReload();
    int getActiveSFXID();
    
    virtual ~CustomSFXCell();

protected:
    SFXInfoObject* m_sfxObject;
    bool m_compactMode;
    bool m_selected;
    int m_listType;
};