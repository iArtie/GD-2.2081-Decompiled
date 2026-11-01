#pragma once

#include "Utils/GJEnums.h"

class LikeItemDelegate {
public:
    void likedItem(LikeItemType type, int id, bool liked);
};