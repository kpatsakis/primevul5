lyxml_free_withsiblings(struct ly_ctx *ctx, struct lyxml_elem *elem)
{
    FUN_IN;

    struct lyxml_elem *iter, *aux;

    if (!elem) {
        return;
    }

    /* optimization - avoid freeing (unlinking) the last node of the siblings list */
    /* so, first, free the node's predecessors to the beginning of the list ... */
    for(iter = elem->prev; iter->next; iter = aux) {
        aux = iter->prev;
        lyxml_free(ctx, iter);
    }
    /* ... then, the node is the first in the siblings list, so free them all */
    LY_TREE_FOR_SAFE(elem, aux, iter) {
        lyxml_free(ctx, iter);
    }
}