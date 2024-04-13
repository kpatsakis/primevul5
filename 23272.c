void mobi_buffer_copy8(MOBIBuffer *dest, MOBIBuffer *source) {
    mobi_buffer_add8(dest, mobi_buffer_get8(source));
}