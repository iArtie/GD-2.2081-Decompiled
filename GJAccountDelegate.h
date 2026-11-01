#pragma once

class GJAccountDelegate
{
public:
    virtual void accountStatusChanged();
    virtual ~GJAccountDelegate() {}
};