bool mobi_buffer_match_magic_offset(MOBIBuffer *buf, const char *magic, const size_t offset) {
    bool match = false;
    if (offset <= buf->maxlen) {
        const size_t save_offset = buf->offset;
        buf->offset = offset;
        match = mobi_buffer_match_magic(buf, magic);
        buf->offset = save_offset;
    }
    return match;
}