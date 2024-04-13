static inline int p_buf_refill(struct parser_buf *buf, const int input_fd)
{
    int read_size;

    read_size = read(input_fd, buf->f_buf, SRC_PARSER_F_BUF_SIZE);
    buf->f_indx = 0;
    buf->f_read_size = read_size;
    return read_size;
}