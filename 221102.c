yin_read_submodule(struct lys_module *module, const char *data, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    struct lyxml_elem *yin;
    struct lys_submodule *submodule = NULL;
    const char *value;

    yin = lyxml_parse_mem(ctx, data, LYXML_PARSE_NOMIXEDCONTENT);
    if (!yin) {
        return NULL;
    }

    /* check root element */
    if (!yin->name || strcmp(yin->name, "submodule")) {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, yin->name);
        goto error;
    }

    GETVAL(ctx, value, yin, "name");
    if (lyp_check_identifier(ctx, value, LY_IDENT_NAME, NULL, NULL)) {
        goto error;
    }

    submodule = calloc(1, sizeof *submodule);
    LY_CHECK_ERR_GOTO(!submodule, LOGMEM(ctx), error);

    submodule->ctx = ctx;
    submodule->name = lydict_insert(ctx, value, strlen(value));
    submodule->type = 1;
    submodule->implemented = module->implemented;
    submodule->belongsto = module;

    /* add into the list of processed modules */
    if (lyp_check_circmod_add((struct lys_module *)submodule)) {
        goto error;
    }

    LOGVRB("Reading submodule \"%s\".", submodule->name);
    /* module cannot be changed in this case and 1 cannot be returned */
    if (read_sub_module(module, submodule, yin, unres)) {
        goto error;
    }

    lyp_sort_revisions((struct lys_module *)submodule);

    /* cleanup */
    lyxml_free(ctx, yin);
    lyp_check_circmod_pop(ctx);

    LOGVRB("Submodule \"%s\" successfully parsed.", submodule->name);
    return submodule;

error:
    /* cleanup */
    lyxml_free(ctx, yin);
    if (!submodule) {
        LOGERR(ctx, ly_errno, "Submodule parsing failed.");
        return NULL;
    }

    LOGERR(ctx, ly_errno, "Submodule \"%s\" parsing failed.", submodule->name);

    unres_schema_free((struct lys_module *)submodule, &unres, 0);
    lyp_check_circmod_pop(ctx);
    lys_sub_module_remove_devs_augs((struct lys_module *)submodule);
    lys_submodule_module_data_free(submodule);
    lys_submodule_free(submodule, NULL);
    return NULL;
}