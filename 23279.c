void mobi_buffer_dup16(uint16_t **val, MOBIBuffer *buf) {
    *val = NULL;
    if (buf->offset + 2 > buf->maxlen) {
        return;
    }
    *val = malloc(sizeof(uint16_t));
    if (*val == NULL) {
        return;
    }
    **val = mobi_buffer_get16(buf);
}