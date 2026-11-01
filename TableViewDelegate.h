#pragma once

class CCIndexPath;
class TableView;
class TableViewCell;

class TableViewDelegate {
public:
    virtual void didEndTweenToIndexPath(CCIndexPath* indexPath, TableView* tableView);
    virtual void didSelectRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView);
    virtual float cellHeightForRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView);
    virtual void TableViewWillDisplayCellForRowAtIndexPath(CCIndexPath* indexPath, TableViewCell* cell, TableView* tableView);
    virtual void TableViewDidDisplayCellForRowAtIndexPath(CCIndexPath* indexPath, TableViewCell* cell, TableView* tableView);
    virtual void TableViewWillReloadCellForRowAtIndexPath(CCIndexPath* indexPath, TableViewCell* cell, TableView* tableView);
};