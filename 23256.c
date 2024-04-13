void mobi_buffer_add8(MOBIBuffer *buf, const uint8_t data) {
    if (buf->offset + 1 > buf->maxlen) {
        debug_print("%s", "Buffer full\n");
        buf->error = MOBI_BUFFER_END;
        return;
    }
    buf->data[buf->offset++] = data;
}