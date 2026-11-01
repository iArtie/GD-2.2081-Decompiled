#pragma once

#include <string>

class ShareCommentDelegate {
public:
    void shareCommentClosed(std::string text, ShareCommentLayer *layer);
};
