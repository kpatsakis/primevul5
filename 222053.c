static int get_tag_value(char *buf, int buf_size,
                         const char *str, const char *tag,
                         const char *stopchars)
{
    const char *p;
    char *q;
    p = strstr(str, tag);
    if (!p)
        return -1;
    p += strlen(tag);
    while (qemu_isspace(*p))
        p++;
    q = buf;
    while (*p != '\0' && !strchr(stopchars, *p)) {
        if ((q - buf) < (buf_size - 1))
            *q++ = *p;
        p++;
    }
    *q = '\0';
    return q - buf;
}