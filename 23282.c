uint16_t mobi_buffer_get16(MOBIBuffer *buf) {
    if (buf->offset + 2 > buf->maxlen) {
        debug_print("%s", "End of buffer\n");
        buf->error = MOBI_BUFFER_END;
        return 0;
    }
    uint16_t val;
    val = (uint16_t)((uint16_t) buf->data[buf->offset] << 8 | (uint16_t) buf->data[buf->offset + 1]);
    buf->offset += 2;
    return val;
}