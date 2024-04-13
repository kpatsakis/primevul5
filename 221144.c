fill_yin_iffeature(struct lys_node *parent, int parent_is_feature, struct lyxml_elem *yin, struct lys_iffeature *iffeat,
                   struct unres_schema *unres)
{
    int r, c_ext = 0;
    const char *value;
    struct lyxml_elem *node, *next;
    struct ly_ctx *ctx = parent->module->ctx;

    GETVAL(ctx, value, yin, "name");

    if ((lys_node_module(parent)->version != 2) && ((value[0] == '(') || strchr(value, ' '))) {
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "if-feature");
error:
        return EXIT_FAILURE;
    }

    if (!(value = transform_iffeat_schema2json(parent->module, value))) {
        return EXIT_FAILURE;
    }

    r = resolve_iffeature_compile(iffeat, value, parent, parent_is_feature, unres);
    lydict_remove(ctx, value);
    if (r) {
        return EXIT_FAILURE;
    }

    LY_TREE_FOR_SAFE(yin->child, next, node) {
        if (!node->ns) {
            /* garbage */
            lyxml_free(ctx, node);
        } else if (strcmp(node->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_RETURN(ctx, c_ext, iffeat->ext_size, "extensions", "if-feature", EXIT_FAILURE);
            c_ext++;
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name, "if-feature");
            return EXIT_FAILURE;
        }
    }
    if (c_ext) {
        iffeat->ext = calloc(c_ext, sizeof *iffeat->ext);
        LY_CHECK_ERR_RETURN(!iffeat->ext, LOGMEM(ctx), EXIT_FAILURE);
        LY_TREE_FOR_SAFE(yin->child, next, node) {
            /* extensions */
            r = lyp_yin_fill_ext(iffeat, LYEXT_PAR_IDENT, 0, 0, parent->module, node,
                                 &iffeat->ext, &iffeat->ext_size, unres);
            if (r) {
                return EXIT_FAILURE;
            }
        }

        lyp_reduce_ext_list(&iffeat->ext, iffeat->ext_size, c_ext + iffeat->ext_size);
    }

    return EXIT_SUCCESS;
}