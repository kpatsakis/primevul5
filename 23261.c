uint8_t mobi_buffer_get8(MOBIBuffer *buf) {
    if (buf->offset + 1 > buf->maxlen) {
        debug_print("%s", "End of buffer\n");
        buf->error = MOBI_BUFFER_END;
        return 0;
    }
    return buf->data[buf->offset++];
}