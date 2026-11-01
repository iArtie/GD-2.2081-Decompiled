#pragma once

class TableViewCellDelegate {
public:
    virtual int getCellDelegateType();
    virtual int getSelectedCellIdx();
    virtual bool shouldSnapToSelected();
};