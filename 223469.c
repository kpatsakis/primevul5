dump_siblings(struct lyout *out, const struct lyxml_elem *e, int options)
{
    const struct lyxml_elem *start, *iter, *next;
    int ret = 0;

    if (e->parent) {
        start = e->parent->child;
    } else {
        start = e;
        while(start->prev && start->prev->next) {
            start = start->prev;
        }
    }

    LY_TREE_FOR_SAFE(start, next, iter) {
        ret += dump_elem(out, iter, 0, options, (next ? 0 : 1));
    }

    return ret;
}