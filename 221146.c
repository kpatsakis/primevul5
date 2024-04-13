str_utf8_offset(const char *p, const char *e, long nth)
{
    const char *pp = str_utf8_nth(p, e, nth);
    return pp - p;
}