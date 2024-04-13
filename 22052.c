stprintf(stream * s, const char *fmt, ...)
{
    int count;
    va_list args;
    char buf[1024];

    va_start(args, fmt);
    count = gs_vsprintf(buf, fmt, args);
    if (count >= 0) {
        unsigned count_u = count;
        sputs(s, (const byte *)buf, count_u, &count_u);
    }
    va_end(args);
}