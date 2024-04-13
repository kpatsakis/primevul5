void mobi_buffer_dup32(uint32_t **val, MOBIBuffer *buf) {
    *val = NULL;
    if (buf->offset + 4 > buf->maxlen) {
        return;
    }
    *val = malloc(sizeof(uint32_t));
    if (*val == NULL) {
        return;
    }
    **val = mobi_buffer_get32(buf);
}