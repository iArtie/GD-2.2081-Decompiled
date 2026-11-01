#include "Platform/xml_string_writer.h"

std::string* xml_string_writer::write(const unsigned char* data, size_t size)
{
    std::string value(reinterpret_cast<const char*>(data), size);
    m_buffer += value;
    return &m_buffer;
}

xml_string_writer::~xml_string_writer()
{
}