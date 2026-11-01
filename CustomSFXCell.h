#pragma once

#include "cocos2d.h"
#include "TableViewCell.h"
#include "CustomSFXDelegate.h"
#include "SFXInfoObject.h"

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