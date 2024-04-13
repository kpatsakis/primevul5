void mobi_buffer_add16(MOBIBuffer *buf, const uint16_t data) {
    if (buf->offset + 2 > buf->maxlen) {
        debug_print("%s", "Buffer full\n");
        buf->error = MOBI_BUFFER_END;
        return;
    }
    unsigned char *buftr = buf->data + buf->offset;
    *buftr++ = (uint8_t)((uint32_t)(data & 0xff00U) >> 8);
    *buftr = (uint8_t)((uint32_t)(data & 0xffU));
    buf->offset += 2;
}