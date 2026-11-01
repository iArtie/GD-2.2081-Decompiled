#include "Delegates/TableViewDelegate.h"
#include "Cocos/CCIndexPath.h"
#include "UI/TableView.h"
#include "Cells/TableViewCell.h"

void TableViewDelegate::didEndTweenToIndexPath(CCIndexPath* indexPath, TableView* tableView)
{
}

void TableViewDelegate::didSelectRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView)
{
}

float TableViewDelegate::cellHeightForRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView)
{
    return 40.0f;
}

void TableViewDelegate::TableViewWillDisplayCellForRowAtIndexPath(CCIndexPath* indexPath, TableViewCell* cell, TableView* tableView)
{
}

void TableViewDelegate::TableViewDidDisplayCellForRowAtIndexPath(CCIndexPath* indexPath, TableViewCell* cell, TableView* tableView)
{
}

void TableViewDelegate::TableViewWillReloadCellForRowAtIndexPath(CCIndexPath* indexPath, TableViewCell* cell, TableView* tableView)
{
}