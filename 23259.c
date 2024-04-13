bool mobi_buffer_match_magic(MOBIBuffer *buf, const char *magic) {
    const size_t magic_length = strlen(magic);
    if (buf->offset + magic_length > buf->maxlen) {
        return false;
    }
    if (memcmp(buf->data + buf->offset, magic, magic_length) == 0) {
        return true;
    }
    return false;
}