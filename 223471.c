parse_ignore(struct ly_ctx *ctx, const char *data, const char *endstr, unsigned int *len)
{
    unsigned int slen;
    const char *c = data;

    slen = strlen(endstr);

    while (*c && strncmp(c, endstr, slen)) {
        c++;
    }
    if (!*c) {
        LOGVAL(ctx, LYE_XML_MISS, LY_VLOG_NONE, NULL, "closing sequence", endstr);
        return EXIT_FAILURE;
    }
    c += slen;

    *len = c - data;
    return EXIT_SUCCESS;
}