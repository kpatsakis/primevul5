int bcf_hdr_printf(bcf_hdr_t *hdr, const char *fmt, ...)
{
    char tmp[256], *line = tmp;
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(line, sizeof(tmp), fmt, ap);
    va_end(ap);

    if (n >= sizeof(tmp)) {
        n++; // For trailing NUL
        line = (char*)malloc(n);
        if (!line)
            return -1;

        va_start(ap, fmt);
        vsnprintf(line, n, fmt, ap);
        va_end(ap);
    }

    int ret = bcf_hdr_append(hdr, line);

    if (line != tmp) free(line);
    return ret;
}