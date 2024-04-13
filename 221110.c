fill_yin_must(struct lys_module *module, struct lyxml_elem *yin, struct lys_restr *must, struct unres_schema *unres)
{
    int ret = EXIT_FAILURE;
    const char *value;

    must->expr = NULL;
    GETVAL(module->ctx, value, yin, "condition");
    must->expr = transform_schema2json(module, value);
    if (!must->expr) {
        goto error;
    }

    ret = read_restr_substmt(module, must, yin, unres);

error:
    if (ret) {
        lydict_remove(module->ctx, must->expr);
        must->expr = NULL;
    }
    return ret;
}