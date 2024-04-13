lyxml_dup_attr(struct ly_ctx *ctx, struct lyxml_elem *parent, struct lyxml_attr *attr)
{
    struct lyxml_attr *result, *a;

    if (!attr || !parent) {
        return NULL;
    }

    if (attr->type == LYXML_ATTR_NS) {
        /* this is correct, despite that all attributes seems like a standard
         * attributes (struct lyxml_attr), some of them can be namespace
         * definitions (and in that case they are struct lyxml_ns).
         */
        result = (struct lyxml_attr *)calloc(1, sizeof (struct lyxml_ns));
    } else {
        result = calloc(1, sizeof (struct lyxml_attr));
    }
    LY_CHECK_ERR_RETURN(!result, LOGMEM(ctx), NULL);

    result->value = lydict_insert(ctx, attr->value, 0);
    result->name = lydict_insert(ctx, attr->name, 0);
    result->type = attr->type;

    /* set namespace in case of standard attributes */
    if (result->type == LYXML_ATTR_STD && attr->ns) {
        result->ns = attr->ns;
        lyxml_correct_attr_ns(ctx, result, parent, 1);
    }

    /* set parent pointer in case of namespace attribute */
    if (result->type == LYXML_ATTR_NS) {
        ((struct lyxml_ns *)result)->parent = parent;
    }

    /* put attribute into the parent's attributes list */
    if (parent->attr) {
        /* go to the end of the list */
        for (a = parent->attr; a->next; a = a->next);
        /* and append new attribute */
        a->next = result;
    } else {
        /* add the first attribute in the list */
        parent->attr = result;
    }

    return result;
}