yin_read_module_(struct ly_ctx *ctx, struct lyxml_elem *yin, const char *revision, int implement)
{
    struct lys_module *module = NULL;
    struct unres_schema *unres;
    const char *value;
    int ret;
    uint8_t i;

    unres = calloc(1, sizeof *unres);
    LY_CHECK_ERR_RETURN(!unres, LOGMEM(ctx), NULL);

    /* check root element */
    if (!yin->name || strcmp(yin->name, "module")) {
        if (ly_strequal("submodule", yin->name, 0)) {
            LOGVAL(ctx, LYE_SUBMODULE, LY_VLOG_NONE, NULL);
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, yin->name);
        }
        goto error;
    }

    GETVAL(ctx, value, yin, "name");
    if (lyp_check_identifier(ctx, value, LY_IDENT_NAME, NULL, NULL)) {
        goto error;
    }

    /* in some really invalid situations there can be a circular import and
     * we can check it only after we have parsed the module name */
    for (i = 0; i < ctx->models.parsing_sub_modules_count; ++i) {
        if (!strcmp(ctx->models.parsing_sub_modules[i]->name, value)) {
            LOGVAL(ctx, LYE_CIRC_IMPORTS, LY_VLOG_NONE, NULL, value);
            goto error;
        }
    }

    module = calloc(1, sizeof *module);
    LY_CHECK_ERR_GOTO(!module, LOGMEM(ctx), error);

    module->ctx = ctx;
    module->name = lydict_insert(ctx, value, strlen(value));
    module->type = 0;
    module->implemented = (implement ? 1 : 0);

    /* add into the list of processed modules */
    if (lyp_check_circmod_add(module)) {
        goto error;
    }

    LOGVRB("Reading module \"%s\".", module->name);
    ret = read_sub_module(module, NULL, yin, unres);
    if (ret == -1) {
        goto error;
    }

    if (ret == 1) {
        assert(!unres->count);
    } else {
        /* make this module implemented if was not from start */
        if (!implement && module->implemented && (unres_schema_add_node(module, unres, NULL, UNRES_MOD_IMPLEMENT, NULL) == -1)) {
            goto error;
        }

        /* resolve rest of unres items */
        if (unres->count && resolve_unres_schema(module, unres)) {
            goto error;
        }

        /* check correctness of includes */
        if (lyp_check_include_missing(module)) {
            goto error;
        }
    }

    lyp_sort_revisions(module);

    if (lyp_rfn_apply_ext(module) || lyp_deviation_apply_ext(module)) {
        goto error;
    }

    if (revision) {
        /* check revision of the parsed model */
        if (!module->rev_size || strcmp(revision, module->rev[0].date)) {
            LOGVRB("Module \"%s\" parsed with the wrong revision (\"%s\" instead \"%s\").",
                   module->name, module->rev[0].date, revision);
            goto error;
        }
    }

    /* add into context if not already there */
    if (!ret) {
        if (lyp_ctx_add_module(module)) {
            goto error;
        }

        /* remove our submodules from the parsed submodules list */
        lyp_del_includedup(module, 0);
    } else {
        /* free what was parsed */
        lys_free(module, NULL, 0, 0);

        /* get the model from the context */
        module = (struct lys_module *)ly_ctx_get_module(ctx, value, revision, 0);
        assert(module);
    }

    unres_schema_free(NULL, &unres, 0);
    lyp_check_circmod_pop(ctx);
    LOGVRB("Module \"%s%s%s\" successfully parsed as %s.", module->name, (module->rev_size ? "@" : ""),
           (module->rev_size ? module->rev[0].date : ""), (module->implemented ? "implemented" : "imported"));
    return module;

error:
    /* cleanup */
    unres_schema_free(module, &unres, 1);

    if (!module) {
        if (ly_vecode(ctx) != LYVE_SUBMODULE) {
            LOGERR(ctx, ly_errno, "Module parsing failed.");
        }
        return NULL;
    }

    LOGERR(ctx, ly_errno, "Module \"%s\" parsing failed.", module->name);

    lyp_check_circmod_pop(ctx);
    lys_sub_module_remove_devs_augs(module);
    lyp_del_includedup(module, 1);
    lys_free(module, NULL, 0, 1);
    return NULL;
}