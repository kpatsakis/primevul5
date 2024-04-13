lyp_yin_fill_ext(void *parent, LYEXT_PAR parent_type, LYEXT_SUBSTMT substmt, uint8_t substmt_index,
             struct lys_module *module, struct lyxml_elem *yin, struct lys_ext_instance ***ext,
             uint8_t *ext_size, struct unres_schema *unres)
{
    struct unres_ext *info;
    int rc;

    info = malloc(sizeof *info);
    LY_CHECK_ERR_RETURN(!info, LOGMEM(module->ctx), EXIT_FAILURE);
    lyxml_unlink(module->ctx, yin);
    info->data.yin = yin;
    info->datatype = LYS_IN_YIN;
    info->parent = parent;
    info->mod = module;
    info->parent_type = parent_type;
    info->substmt = substmt;
    info->substmt_index = substmt_index;
    info->ext_index = *ext_size;

    rc = unres_schema_add_node(module, unres, ext, UNRES_EXT, (struct lys_node *)info);
    if (!rc && !(*ext)[*ext_size]) {
        /* extension instance is skipped */
    } else {
        ++(*ext_size);
    }

    return rc == -1 ? EXIT_FAILURE : EXIT_SUCCESS;
}