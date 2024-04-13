void mobi_buffer_dup8(uint8_t **val, MOBIBuffer *buf) {
    *val = NULL;
    if (buf->offset + 1 > buf->maxlen) {
        return;
    }
    *val = malloc(sizeof(uint8_t));
    if (*val == NULL) {
        return;
    }
    **val = mobi_buffer_get8(buf);
}