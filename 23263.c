static uint32_t _buffer_get_varlen(MOBIBuffer *buf, size_t *len, const int direction) {
    uint32_t val = 0;
    uint8_t byte_count = 0;
    uint8_t byte;
    const uint8_t stop_flag = 0x80;
    const uint8_t mask = 0x7f;
    uint32_t shift = 0;
    do {
        if (direction == 1) {
            if (buf->offset + 1 > buf->maxlen) {
                debug_print("%s", "End of buffer\n");
                buf->error = MOBI_BUFFER_END;
                return val;
            }
            byte = buf->data[buf->offset++];
            val <<= 7;
            val |= (byte & mask);
        } else {
            if (buf->offset < 1) {
                debug_print("%s", "End of buffer\n");
                buf->error = MOBI_BUFFER_END;
                return val;
            }
            byte = buf->data[buf->offset--];
            val = val | (uint32_t)(byte & mask) << shift;
            shift += 7;
        }        
        (*len)++;
        byte_count++;
    } while (!(byte & stop_flag) && (byte_count < 4));
    return val;
}