#pragma once

#include <cstddef>
#include <string>

class xml_string_writer {
public:
    std::string* write(const unsigned char* data, size_t size);
    ~xml_string_writer();

protected:
    std::string m_buffer;
};