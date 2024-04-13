static size_t strnlen(const char *s, size_t maxlen) {
        char *r = (char *)memchr(s, '\0', maxlen);
        return r ? r-s : maxlen;
}