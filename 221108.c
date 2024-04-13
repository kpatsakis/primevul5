yin_getplace_for_extcomplex_struct(struct lyxml_elem *node, struct lys_ext_instance_complex *ext, LY_STMT stmt)
{
    int c;
    void **data, ***p = NULL;
    void *reallocated;
    struct lyext_substmt *info;

    data = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!data) {
        LOGVAL(ext->module->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return NULL;
    }
    if (info->cardinality < LY_STMT_CARD_SOME && *data) {
        LOGVAL(ext->module->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return NULL;
    }

    c = 0;
    if (info->cardinality >= LY_STMT_CARD_SOME) {
        /* there can be multiple instances, so instead of pointer to array,
         * we have in data pointer to pointer to array */
        p = (void ***)data;
        data = *p;
        if (!data) {
            /* allocate initial array */
            *p = data = malloc(2 * sizeof(void *));
            LY_CHECK_ERR_RETURN(!data, LOGMEM(ext->module->ctx), NULL);
        } else {
            for (c = 0; *data; data++, c++);
        }
    }

    if (p) {
        /* enlarge the array */
        reallocated = realloc(*p, (c + 2) * sizeof(void *));
        LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(ext->module->ctx), NULL);
        *p = reallocated;
        data = *p;
        data[c + 1] = NULL;
    }

    return &data[c];
}