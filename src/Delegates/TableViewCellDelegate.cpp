#include "Delegates/TableViewCellDelegate.h"

int TableViewCellDelegate::getCellDelegateType()
{
    return 0;
}

int TableViewCellDelegate::getSelectedCellIdx()
{
    return 0;
}

bool TableViewCellDelegate::shouldSnapToSelected()
{
    return true;
}