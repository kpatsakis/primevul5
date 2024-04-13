static inline int p_buf_push_tmp_char(struct parser_buf *buf, const char c)
{
    buf->tmp_buf[buf->tmp_indx++] = c;
    buf->f_indx++;
    return buf->tmp_indx;
}