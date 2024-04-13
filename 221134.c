yin_read_module(struct ly_ctx *ctx, const char *data, const char *revision, int implement)
{
    struct lyxml_elem *yin;
    struct lys_module *result;

    yin = lyxml_parse_mem(ctx, data, LYXML_PARSE_NOMIXEDCONTENT);
    if (!yin) {
        LOGERR(ctx, ly_errno, "Module parsing failed.");
        return NULL;
    }

    result = yin_read_module_(ctx, yin, revision, implement);

    lyxml_free(ctx, yin);

    return result;
}