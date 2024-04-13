png_path_to_bitmap(const char* path, uint8_t** data, unsigned int* width, unsigned int* height, size_t* sz) {
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        log_error("The PNG image: %s could not be opened with error: %s", path, strerror(errno));
        return false;
    }
    size_t capacity = 16*1024, pos = 0;
    unsigned char *buf = malloc(capacity);
    if (!buf) { log_error("Out of memory reading PNG file at: %s", path); fclose(fp); return false; }
    while (!feof(fp)) {
        if (pos - capacity < 1024) {
            capacity *= 2;
            unsigned char *new_buf = realloc(buf, capacity);
            if (!new_buf) {
                free(buf);
                log_error("Out of memory reading PNG file at: %s", path); fclose(fp); return false;
            }
            buf = new_buf;
        }
        pos += fread(buf + pos, sizeof(char), capacity - pos, fp);
        int saved_errno = errno;
        if (ferror(fp) && saved_errno != EINTR) {
            log_error("Failed while reading from file: %s with error: %s", path, strerror(saved_errno));
            fclose(fp);
            free(buf);
            return false;
        }
    }
    fclose(fp); fp = NULL;
    png_read_data d = {0};
    inflate_png_inner(&d, buf, pos);
    free(buf);
    if (!d.ok) {
        log_error("Failed to decode PNG image at: %s", path);
        return false;
    }
    *data = d.decompressed;
    *sz = d.sz;
    *height = d.height; *width = d.width;
    return true;
}