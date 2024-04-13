read_restr_substmt(struct lys_module *module, struct lys_restr *restr, struct lyxml_elem *yin,
                   struct unres_schema *unres)
{
    struct lyxml_elem *child, *next;
    const char *value;
    struct ly_ctx *ctx = module->ctx;

    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!child->ns) {
            /* garbage */
            continue;
        } else if (strcmp(child->ns->value, LY_NSYIN)) {
            /* extension */
            if (lyp_yin_parse_subnode_ext(module, restr, LYEXT_PAR_RESTR, child, LYEXT_SUBSTMT_SELF, 0, unres)) {
                return EXIT_FAILURE;
            }
        } else if (!strcmp(child->name, "description")) {
            if (restr->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                return EXIT_FAILURE;
            }
            if (lyp_yin_parse_subnode_ext(module, restr, LYEXT_PAR_RESTR, child, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                return EXIT_FAILURE;
            }
            restr->dsc = read_yin_subnode(ctx, child, "text");
            if (!restr->dsc) {
                return EXIT_FAILURE;
            }
        } else if (!strcmp(child->name, "reference")) {
            if (restr->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                return EXIT_FAILURE;
            }
            if (lyp_yin_parse_subnode_ext(module, restr, LYEXT_PAR_RESTR, child, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                return EXIT_FAILURE;
            }
            restr->ref = read_yin_subnode(ctx, child, "text");
            if (!restr->ref) {
                return EXIT_FAILURE;
            }
        } else if (!strcmp(child->name, "error-app-tag")) {
            if (restr->eapptag) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                return EXIT_FAILURE;
            }
            if (lyp_yin_parse_subnode_ext(module, restr, LYEXT_PAR_RESTR, child, LYEXT_SUBSTMT_ERRTAG, 0, unres)) {
                return EXIT_FAILURE;
            }
            GETVAL(ctx, value, child, "value");
            restr->eapptag = lydict_insert(ctx, value, 0);
        } else if (!strcmp(child->name, "error-message")) {
            if (restr->emsg) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                return EXIT_FAILURE;
            }
            if (lyp_yin_parse_subnode_ext(module, restr, LYEXT_PAR_RESTR, child, LYEXT_SUBSTMT_ERRMSG, 0, unres)) {
                return EXIT_FAILURE;
            }
            restr->emsg = read_yin_subnode(ctx, child, "value");
            if (!restr->emsg) {
                return EXIT_FAILURE;
            }
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}