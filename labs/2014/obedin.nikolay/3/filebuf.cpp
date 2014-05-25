#include "filebuf.hpp"

int ofilebuf::flush()
{
    int cur_buf_size = pptr() - pbase();
    if (cur_buf_size == 0)
        return cur_buf_size;

    if (m_cur_block_num == filesystem::NO_FREE_BLOCKS) {
        is_good = false;
        return EOF;
    }

    ofstream out(m_fs->block_path(m_cur_block_num), ios::binary | ios::trunc);
    if (!out.is_open()) {
        is_good = false;
        return EOF;
    }

    m_fs->mark_block(m_cur_block_num, filesystem::USED);
    m_cur_block_num = m_fs->next_free_block_num();

    char next_block[sizeof(block_num)];
    for (size_t i = 0; i < sizeof(block_num); i++) {
        int offset = 8 * (sizeof(block_num) - i - 1);
        next_block[i] = (m_cur_block_num >> offset) & 0xFF;
    }

    out.write(m_buffer, cur_buf_size);
    if (cur_buf_size >= m_buffer_size-1)
        out.write(next_block, (long int)sizeof(block_num));
    pbump(-cur_buf_size);

    return cur_buf_size;
}

int ofilebuf::overflow(int c)
{
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    return (flush() == EOF) ? EOF : c;
}

int ifilebuf::underflow()
{
    if (gptr() < egptr())
        return *gptr();

    if (m_bytes_read >= m_size)
        return EOF;

    ifstream in(m_fs->block_path(m_cur_block_num), ios::binary);
    if (!in.is_open()) {
        is_good = false;
        return EOF;
    }

    long int bytes_remaining = m_size - m_bytes_read;
    if (bytes_remaining > (long int)m_fs->block_size()) {
        bytes_remaining = m_fs->block_size() - sizeof(block_num);
        in.read(m_buffer, bytes_remaining);
        bytes_remaining = in.gcount();

        unsigned char next_block[sizeof(block_num)];
        in.read((char*)next_block, sizeof(block_num));
        m_cur_block_num = 0;
        for (size_t i = 0; i < sizeof(block_num); i++) {
            block_num n = 0;
            n = next_block[i];
            int offset = 8 * (sizeof(block_num) - i - 1);
            n = n << offset;
            m_cur_block_num |= n;
        }

        if (m_collect_blocks)
            blocks.push_back(m_cur_block_num);
    } else {
        in.read(m_buffer, bytes_remaining);
        bytes_remaining = in.gcount();
        m_cur_block_num = filesystem::NO_FREE_BLOCKS;
    }

    m_bytes_read += bytes_remaining;
    setg(m_buffer, m_buffer, m_buffer+bytes_remaining);
    return traits_type::to_int_type(*gptr());
}
