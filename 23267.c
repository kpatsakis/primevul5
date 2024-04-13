void mobi_buffer_appendstring(char *str, MOBIBuffer *buf, const size_t len) {
    if (!str) {
        buf->error = MOBI_PARAM_ERR;
        return;
    }
    if (buf->offset + len > buf->maxlen) {
        debug_print("%s", "End of buffer\n");
        buf->error = MOBI_BUFFER_END;
        return;
    }
    size_t str_len = strlen(str);
    memcpy(str + str_len, buf->data + buf->offset, len);
    str[str_len + len] = '\0';
    buf->offset += len;
}