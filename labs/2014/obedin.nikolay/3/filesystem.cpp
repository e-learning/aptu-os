#include "filesystem.hpp"

filesystem::filesystem(const string &path)
    : m_path(path), m_block_size(0), m_blocks_count(0)
{
    m_path = m_path.back() == '/' ? m_path : m_path + "/";

    fstream in(m_path + "config", fstream::in);
    if (!in.is_open())
        throw error("config file is not found");

    in >> m_block_size >> m_blocks_count;
    if (!is_valid())
        throw error("incorrect configuration");
}

bool filesystem::is_valid() const
{
    return m_block_size >= 1024 && m_blocks_count > 0;
}

void filesystem::init()
{
    for (size_t i = 0; i < m_blocks_count; ++i) {
        fstream out(m_path + to_string(i), fstream::out);
        out.flush();
    }
    fstream out(m_path + "metadata", fstream::out);
    out.flush();
}
