#include "Cocos/CCIndexPath.h"

CCIndexPath::CCIndexPath() {
    m_row = 0;
    m_section = 0;
}

CCIndexPath::CCIndexPath(int row, int section) {
    m_row = row;
    m_section = section;
}

CCIndexPath::~CCIndexPath() {
    // Destructor
}