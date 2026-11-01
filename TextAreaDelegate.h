#pragma once

class TextArea;

class TextAreaDelegate
{
public:
    virtual void fadeInTextFinished(TextArea* textArea);
    
    virtual ~TextAreaDelegate() {}
};