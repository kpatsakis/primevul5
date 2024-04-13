uint32_t mobi_buffer_get_varlen_dec(MOBIBuffer *buf, size_t *len) {
    return _buffer_get_varlen(buf, len, -1);
}