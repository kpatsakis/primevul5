fill_yin_unique(struct lys_module *module, struct lys_node *parent, struct lyxml_elem *yin, struct lys_unique *unique,
                struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    int i, j, ret = EXIT_FAILURE;
    const char *orig;
    char *value, *vaux, *start = NULL, c = 0;
    struct unres_list_uniq *unique_info;

    /* get unique value (list of leafs supposed to be unique */
    GETVAL(ctx, orig, yin, "tag");

    /* count the number of unique leafs in the value */
    start = value = vaux = strdup(orig);
    LY_CHECK_ERR_GOTO(!vaux, LOGMEM(ctx), error);
    while ((vaux = strpbrk(vaux, " \t\n"))) {
        YIN_CHECK_ARRAY_OVERFLOW_CODE(ctx, unique->expr_size, unique->expr_size, "referenced items", "unique",
                                      unique->expr_size = 0; goto error);
        unique->expr_size++;
        while (isspace(*vaux)) {
            vaux++;
        }
    }
    unique->expr_size++;
    unique->expr = calloc(unique->expr_size, sizeof *unique->expr);
    LY_CHECK_ERR_GOTO(!unique->expr, LOGMEM(ctx), error);

    for (i = 0; i < unique->expr_size; i++) {
        vaux = strpbrk(value, " \t\n");
        if (vaux) {
            c = *vaux;
            *vaux = '\0';
        }

        /* store token into unique structure */
        unique->expr[i] = transform_schema2json(module, value);
        if (vaux) {
            *vaux = c;
        }

        /* check that the expression does not repeat */
        for (j = 0; j < i; j++) {
            if (ly_strequal(unique->expr[j], unique->expr[i], 1)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, unique->expr[i], "unique");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "The identifier is not unique");
                goto error;
            }
        }

        /* try to resolve leaf */
        if (unres) {
            unique_info = malloc(sizeof *unique_info);
            LY_CHECK_ERR_GOTO(!unique_info, LOGMEM(ctx), error);
            unique_info->list = parent;
            unique_info->expr = unique->expr[i];
            unique_info->trg_type = &unique->trg_type;
            if (unres_schema_add_node(module, unres, unique_info, UNRES_LIST_UNIQ, NULL) == -1){
                goto error;
            }
        } else {
            if (resolve_unique(parent, unique->expr[i], &unique->trg_type)) {
                goto error;
            }
        }

        /* move to next token */
        value = vaux;
        while (value && isspace(*value)) {
            value++;
        }
    }

    ret =  EXIT_SUCCESS;

error:
    free(start);
    return ret;
}