lyp_yin_parse_subnode_ext(struct lys_module *mod, void *elem, LYEXT_PAR elem_type,
                     struct lyxml_elem *yin, LYEXT_SUBSTMT type, uint8_t i, struct unres_schema *unres)
{
    void *reallocated;
    struct lyxml_elem *next, *child;
    int r;
    struct lys_ext_instance ***ext;
    uint8_t *ext_size;
    const char *statement;

    r = lyp_get_ext_list(mod->ctx, elem, elem_type, &ext, &ext_size, &statement);
    LY_CHECK_RETURN(r, EXIT_FAILURE);

    if (type == LYEXT_SUBSTMT_SELF) {
        /* parse for the statement self, not for the substatement */
        child = yin;
        next = NULL;
        goto parseext;
    }

    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!child->ns) {
            LOGVAL(mod->ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Extension instance \"%s\" is missing namespace.", child->name);
            return EXIT_FAILURE;
        }
        if (!strcmp(child->ns->value, LY_NSYIN)) {
            /* skip the regular YIN nodes */
            continue;
        }

        /* parse it as extension */
parseext:

        YIN_CHECK_ARRAY_OVERFLOW_RETURN(mod->ctx, *ext_size, *ext_size, "extension", statement, EXIT_FAILURE);
        /* first, allocate a space for the extension instance in the parent elem */
        reallocated = realloc(*ext, (1 + (*ext_size)) * sizeof **ext);
        LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(mod->ctx), EXIT_FAILURE);
        (*ext) = reallocated;

        /* init memory */
        (*ext)[(*ext_size)] = NULL;

        /* parse YIN data */
        r = lyp_yin_fill_ext(elem, elem_type, type, i, mod, child, ext, ext_size, unres);
        if (r) {
            return EXIT_FAILURE;
        }

        lyp_reduce_ext_list(ext, *ext_size, 1 + (*ext_size));

        /* done - do not free the child, it is unlinked in lyp_yin_fill_ext */
    }

    return EXIT_SUCCESS;
}