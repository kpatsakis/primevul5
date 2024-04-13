void mobi_buffer_seek(MOBIBuffer *buf, const int diff) {
    size_t adiff = (size_t) abs(diff);
    if (diff >= 0) {
        if (buf->offset + adiff <= buf->maxlen) {
            buf->offset += adiff;
            return;
        }
    } else {
        if (buf->offset >= adiff) {
            buf->offset -= adiff;
            return;
        }
    }
    buf->error = MOBI_BUFFER_END;
    debug_print("%s", "End of buffer\n");
}