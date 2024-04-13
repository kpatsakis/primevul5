read_yin_when(struct lys_module *module, struct lyxml_elem *yin, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    struct lys_when *retval = NULL;
    struct lyxml_elem *child, *next;
    const char *value;

    retval = calloc(1, sizeof *retval);
    LY_CHECK_ERR_RETURN(!retval, LOGMEM(ctx), NULL);

    GETVAL(ctx, value, yin, "condition");
    retval->cond = transform_schema2json(module, value);
    if (!retval->cond) {
        goto error;
    }

    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!child->ns) {
            /* garbage */
            continue;
        } else if (strcmp(child->ns->value, LY_NSYIN)) {
            /* extensions */
            if (lyp_yin_parse_subnode_ext(module, retval, LYEXT_PAR_WHEN, child, LYEXT_SUBSTMT_SELF, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(child->name, "description")) {
            if (retval->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, retval, LYEXT_PAR_WHEN, child, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }

            retval->dsc = read_yin_subnode(ctx, child, "text");
            if (!retval->dsc) {
                goto error;
            }
        } else if (!strcmp(child->name, "reference")) {
            if (retval->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, retval, LYEXT_PAR_WHEN, child, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }

            retval->ref = read_yin_subnode(ctx, child, "text");
            if (!retval->ref) {
                goto error;
            }
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            goto error;
        }
    }

    return retval;

error:
    lys_when_free(ctx, retval, NULL);
    return NULL;
}