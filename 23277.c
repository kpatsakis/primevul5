void mobi_buffer_setpos(MOBIBuffer *buf, const size_t pos) {
    if (pos <= buf->maxlen) {
        buf->offset = pos;
        return;
    }
    buf->error = MOBI_BUFFER_END;
    debug_print("%s", "End of buffer\n");
}