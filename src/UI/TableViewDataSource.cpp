#include "UI/TableViewDataSource.h"
#include "UI/TableView.h"

unsigned int TableViewDataSource::numberOfSectionsInTableView(TableView* tableView)
{
    return 1;
}

int TableViewDataSource::numberOfRowsInSection(unsigned int section, TableView* tableView)
{
    return 0;
}

TableViewCell* TableViewDataSource::cellForRowAtIndexPath(int row, int section, TableView* tableView)
{
    return nullptr;
}

void TableViewDataSource::TableViewCommitCellEditingStyleForRowAtIndexPath(TableView* tableView, int editingStyle, int row, int section)
{
}