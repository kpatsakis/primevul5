lyxml_unlink_elem(struct ly_ctx *ctx, struct lyxml_elem *elem, int copy_ns)
{
    struct lyxml_elem *parent, *first;

    if (!elem) {
        return;
    }

    /* store pointers to important nodes */
    parent = elem->parent;

    /* unlink from parent */
    if (parent) {
        if (parent->child == elem) {
            /* we unlink the first child */
            /* update the parent's link */
            parent->child = elem->next;
        }
        /* forget about the parent */
        elem->parent = NULL;
    }

    if (copy_ns < 2) {
        lyxml_correct_elem_ns(ctx, elem, parent, copy_ns, 1);
    }

    /* unlink from siblings */
    if (elem->prev == elem) {
        /* there are no more siblings */
        return;
    }
    if (elem->next) {
        elem->next->prev = elem->prev;
    } else {
        /* unlinking the last element */
        if (parent) {
            first = parent->child;
        } else {
            first = elem;
            while (first->prev->next) {
                first = first->prev;
            }
        }
        first->prev = elem->prev;
    }
    if (elem->prev->next) {
        elem->prev->next = elem->next;
    }

    /* clean up the unlinked element */
    elem->next = NULL;
    elem->prev = elem;
}