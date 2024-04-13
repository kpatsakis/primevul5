lyxml_free_attrs(struct ly_ctx *ctx, struct lyxml_elem *elem)
{
    struct lyxml_attr *a, *next;
    if (!elem || !elem->attr) {
        return;
    }

    a = elem->attr;
    do {
        next = a->next;

        lydict_remove(ctx, a->name);
        lydict_remove(ctx, a->value);
        if (a->type == LYXML_ATTR_STD_UNRES) {
            free((char *)a->ns);
        }
        free(a);

        a = next;
    } while (a);
}