#pragma once

class DialogLayer;

class DialogDelegate {
public:
    virtual void dialogClosed(DialogLayer* layer);
    virtual ~DialogDelegate() {}
};