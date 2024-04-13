uint32_t mobi_buffer_get32(MOBIBuffer *buf) {
    if (buf->offset + 4 > buf->maxlen) {
        debug_print("%s", "End of buffer\n");
        buf->error = MOBI_BUFFER_END;
        return 0;
    }
    uint32_t val;
    val = (uint32_t) buf->data[buf->offset] << 24 | (uint32_t) buf->data[buf->offset + 1] << 16 | (uint32_t) buf->data[buf->offset + 2] << 8 | (uint32_t) buf->data[buf->offset + 3];
    buf->offset += 4;
    return val;
}