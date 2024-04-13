deviate_minmax(struct lys_node *target, struct lyxml_elem *node, struct lys_deviate *d, int type)
{
    const char *value;
    char *endptr;
    unsigned long val;
    uint32_t *ui32val, *min, *max;
    struct ly_ctx *ctx = target->module->ctx;

    /* del min/max is forbidden */
    if (d->mod == LY_DEVIATE_DEL) {
        LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, (type ? "max-elements" : "min-elements"), "deviate delete");
        goto error;
    }

    /* check target node type */
    if (target->nodetype == LYS_LEAFLIST) {
        max = &((struct lys_node_leaflist *)target)->max;
        min = &((struct lys_node_leaflist *)target)->min;
    } else if (target->nodetype == LYS_LIST) {
        max = &((struct lys_node_list *)target)->max;
        min = &((struct lys_node_list *)target)->min;
    } else {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"%s\" property.", node->name);
        goto error;
    }

    GETVAL(ctx, value, node, "value");
    while (isspace(value[0])) {
        value++;
    }

    if (type && !strcmp(value, "unbounded")) {
        d->max = val = 0;
        d->max_set = 1;
        ui32val = max;
    } else {
        /* convert it to uint32_t */
        errno = 0;
        endptr = NULL;
        val = strtoul(value, &endptr, 10);
        if (*endptr || value[0] == '-' || errno || val > UINT32_MAX) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
            goto error;
        }
        if (type) {
            d->max = (uint32_t)val;
            d->max_set = 1;
            ui32val = max;
        } else {
            d->min = (uint32_t)val;
            d->min_set = 1;
            ui32val = min;
        }
    }

    if (d->mod == LY_DEVIATE_ADD) {
        /* check that there is no current value */
        if (*ui32val) {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
            goto error;
        }
    } else if (d->mod == LY_DEVIATE_RPL) {
        /* unfortunately, there is no way to check reliably that there
         * was a value before, it could have been the default */
    }

    /* add (already checked) and replace */
    /* set new value specified in deviation */
    *ui32val = (uint32_t)val;

    /* check min-elements is smaller than max-elements */
    if (*max && *min > *max) {
        if (type) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "max-elements");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"max-elements\" is smaller than \"min-elements\".");
        } else {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "min-elements");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"min-elements\" is bigger than \"max-elements\".");
        }
        goto error;
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}