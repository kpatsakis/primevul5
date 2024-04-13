void mobi_buffer_addstring(MOBIBuffer *buf, const char *str) {
    const size_t len = strlen(str);
    mobi_buffer_addraw(buf, (const unsigned char *) str, len);
}