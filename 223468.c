lyxml_free_elem(struct ly_ctx *ctx, struct lyxml_elem *elem)
{
    struct lyxml_elem *e, *next;

    if (!elem) {
        return;
    }

    lyxml_free_attrs(ctx, elem);
    LY_TREE_FOR_SAFE(elem->child, next, e) {
        lyxml_free_elem(ctx, e);
    }
    lydict_remove(ctx, elem->name);
    lydict_remove(ctx, elem->content);
    free(elem);
}