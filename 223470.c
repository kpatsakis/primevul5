lyxml_add_child(struct ly_ctx *ctx, struct lyxml_elem *parent, struct lyxml_elem *elem)
{
    struct lyxml_elem *e;

    assert(parent);
    assert(elem);

    /* (re)link element to parent */
    if (elem->parent) {
        lyxml_unlink_elem(ctx, elem, 1);
    }
    elem->parent = parent;

    /* link parent to element */
    if (parent->child) {
        e = parent->child;
        elem->prev = e->prev;
        elem->next = NULL;
        elem->prev->next = elem;
        e->prev = elem;
    } else {
        parent->child = elem;
        elem->prev = elem;
        elem->next = NULL;
    }

    return EXIT_SUCCESS;
}