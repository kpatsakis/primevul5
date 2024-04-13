lyxml_dup_elem(struct ly_ctx *ctx, struct lyxml_elem *elem, struct lyxml_elem *parent, int recursive, int with_siblings)
{
    struct lyxml_elem *dup, *result = NULL;
    struct lyxml_attr *attr;

    if (!elem) {
        return NULL;
    }

    LY_TREE_FOR(elem, elem) {
        dup = calloc(1, sizeof *dup);
        LY_CHECK_ERR_RETURN(!dup, LOGMEM(ctx), NULL);
        dup->content = lydict_insert(ctx, elem->content, 0);
        dup->name = lydict_insert(ctx, elem->name, 0);
        dup->flags = elem->flags;
        dup->prev = dup;

        if (parent) {
            lyxml_add_child(ctx, parent, dup);
        } else if (result) {
            dup->prev = result->prev;
            dup->prev->next = dup;
            result->prev = dup;
        }

        /* keep old namespace for now */
        dup->ns = elem->ns;

        /* duplicate attributes */
        for (attr = elem->attr; attr; attr = attr->next) {
            lyxml_dup_attr(ctx, dup, attr);
        }

        /* correct namespaces */
        lyxml_correct_elem_ns(ctx, dup, elem, 1, 0);

        if (recursive) {
            /* duplicate children */
            lyxml_dup_elem(ctx, elem->child, dup, 1, 1);
        }

        /* set result (first sibling) */
        if (!result) {
            result = dup;
        }

        if (!with_siblings) {
            break;
        }
    }

    return result;
}