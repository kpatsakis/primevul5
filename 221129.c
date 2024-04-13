yin_getplace_for_extcomplex_flags(struct lyxml_elem *node, struct lys_ext_instance_complex *ext, LY_STMT stmt,
                                  uint16_t mask)
{
    void *data;
    struct lyext_substmt *info;

    data = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!data) {
        LOGVAL(ext->module->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return NULL;
    }
    if (info->cardinality < LY_STMT_CARD_SOME && ((*(uint16_t *)data) & mask)) {
        LOGVAL(ext->module->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return NULL;
    }

    return data;
}