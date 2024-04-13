fill_yin_revision(struct lys_module *module, struct lyxml_elem *yin, struct lys_revision *rev,
                  struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    struct lyxml_elem *next, *child;
    const char *value;

    GETVAL(ctx, value, yin, "date");
    if (lyp_check_date(ctx, value)) {
        goto error;
    }
    memcpy(rev->date, value, LY_REV_SIZE - 1);

    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!child->ns) {
            /* garbage */
            continue;
        } else if (strcmp(child->ns->value, LY_NSYIN)) {
            /* possible extension instance */
            if (lyp_yin_parse_subnode_ext(module, rev, LYEXT_PAR_REVISION,
                                          child, LYEXT_SUBSTMT_SELF, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(child->name, "description")) {
            if (rev->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, rev, LYEXT_PAR_REVISION,
                                          child, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }
            rev->dsc = read_yin_subnode(ctx, child, "text");
            if (!rev->dsc) {
                goto error;
            }
        } else if (!strcmp(child->name, "reference")) {
            if (rev->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, rev, LYEXT_PAR_REVISION,
                                          child, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }
            rev->ref = read_yin_subnode(ctx, child, "text");
            if (!rev->ref) {
                goto error;
            }
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}