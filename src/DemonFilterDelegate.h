#pragma once

class DemonFilterDelegate {
public:
    virtual void demonFilterSelectClosed(int filter);
    virtual ~DemonFilterDelegate() {}
};