yin_parse_extcomplex_flag(struct lys_module *mod, struct lyxml_elem *node,
                          struct lys_ext_instance_complex *ext, LY_STMT stmt,
                          const char *val1_str, const char *val2_str, uint16_t mask,
                          uint16_t val1, uint16_t val2, struct unres_schema *unres)
{
    uint16_t *val;
    const char *str;

    val = yin_getplace_for_extcomplex_flags(node, ext, stmt, mask);
    if (!val) {
        return EXIT_FAILURE;
    }

    str = lyxml_get_attr(node, "value", NULL);
    if (!str) {
        LOGVAL(mod->ctx, LYE_MISSARG, LY_VLOG_NONE, NULL, "value", node->name);
    } else if (!strcmp(val1_str, str)) {
        *val = *val | val1;
    } else if (!strcmp(val2_str, str)) {
        *val = *val | val2;
    } else {
        /* unknown value */
        LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, str, node->name);
        return EXIT_FAILURE;
    }
    if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, (LYEXT_SUBSTMT)stmt, 0, unres)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}