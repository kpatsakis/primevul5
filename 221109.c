yin_parse_extcomplex_bool(struct lys_module *mod, struct lyxml_elem *node,
                          struct lys_ext_instance_complex *ext, LY_STMT stmt,
                          const char *true_val, const char *false_val, struct unres_schema *unres)
{
    uint8_t *val;
    const char *str;
    struct lyext_substmt *info;

    val = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!val) {
        LOGVAL(mod->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return EXIT_FAILURE;
    }
    if (*val) {
        LOGVAL(mod->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return EXIT_FAILURE;
    }

    if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, (LYEXT_SUBSTMT)stmt, 0, unres)) {
        return EXIT_FAILURE;
    }

    str = lyxml_get_attr(node, "value", NULL);
    if (!str) {
        LOGVAL(mod->ctx, LYE_MISSARG, LY_VLOG_NONE, NULL, "value", node->name);
    } else if (true_val && !strcmp(true_val, str)) {
        /* true value */
        *val = 1;
    } else if (false_val && !strcmp(false_val, str)) {
        /* false value */
        *val = 2;
    } else {
        /* unknown value */
        LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, str, node->name);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}