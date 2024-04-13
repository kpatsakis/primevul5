void mobi_buffer_addzeros(MOBIBuffer *buf, const size_t count) {
    if (buf->offset + count > buf->maxlen) {
        debug_print("%s", "Buffer full\n");
        buf->error = MOBI_BUFFER_END;
        return;
    }
    memset(buf->data + buf->offset, 0, count);
    buf->offset += count;
}