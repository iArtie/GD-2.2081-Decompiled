#pragma once

class TableView;

class TableViewDataSource {
public:
    virtual unsigned int numberOfSectionsInTableView(TableView* tableView);
    virtual int numberOfRowsInSection(unsigned int section, TableView* tableView);
    virtual TableViewCell* cellForRowAtIndexPath(int row, int section, TableView* tableView);
    virtual void TableViewCommitCellEditingStyleForRowAtIndexPath(TableView* tableView, int editingStyle, int row, int section);
};