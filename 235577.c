static inline int p_buf_write_tmp(struct parser_buf *buf, const int output_fd)
{
    int write_size;

    if (!buf->tmp_indx)
        return 0;

    write_size = write(output_fd, buf->tmp_buf, buf->tmp_indx);
    buf->tmp_indx = 0;

    return write_size;
}