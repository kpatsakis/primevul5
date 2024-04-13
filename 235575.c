static inline int p_buf_write_f_char(struct parser_buf *buf, const int output_fd)
{
    return write(output_fd, &buf->f_buf[buf->f_indx++], 1);
}