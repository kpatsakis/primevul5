void mobi_buffer_getstring(char *str, MOBIBuffer *buf, const size_t len) {
    if (!str) {
        buf->error = MOBI_PARAM_ERR;
        return;
    }
    if (buf->offset + len > buf->maxlen) {
        debug_print("%s", "End of buffer\n");
        buf->error = MOBI_BUFFER_END;
        str[0] = '\0';
        return;
    }
    memcpy(str, buf->data + buf->offset, len);
    str[len] = '\0';
    buf->offset += len;
}