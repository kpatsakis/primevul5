stputs(stream * s, const char *str)
{
    uint ignore_count;

    sputs(s, (const byte *)str, strlen(str), &ignore_count);
}