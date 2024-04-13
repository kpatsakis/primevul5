lyxml_correct_elem_ns(struct ly_ctx *ctx, struct lyxml_elem *elem, struct lyxml_elem *orig, int copy_ns,
                      int correct_attrs)
{
    const struct lyxml_ns *tmp_ns;
    struct lyxml_elem *elem_root, *ns_root, *tmp, *iter;
    struct lyxml_attr *attr;

    /* find the root of elem */
    for (elem_root = elem; elem_root->parent; elem_root = elem_root->parent);

    LY_TREE_DFS_BEGIN(elem, tmp, iter) {
        if (iter->ns) {
            /* find the root of elem NS */
            for (ns_root = iter->ns->parent; ns_root; ns_root = ns_root->parent);

            /* elem NS is defined outside elem subtree */
            if (ns_root != elem_root) {
                if (copy_ns) {
                    tmp_ns = iter->ns;
                    /* we may have already copied the NS over? */
                    iter->ns = lyxml_get_ns(iter, tmp_ns->prefix);

                    /* we haven't copied it over, copy it now */
                    if (!iter->ns) {
                        iter->ns = (struct lyxml_ns *)lyxml_dup_attr(ctx, iter, (struct lyxml_attr *)tmp_ns);
                    }
                } else {
                    iter->ns = NULL;
                }
            }
        }
        if (iter->content && iter->content[0] && copy_ns) {
            lyxml_correct_content_ns(ctx, iter, orig);
        }
        if (correct_attrs) {
            LY_TREE_FOR(iter->attr, attr) {
                lyxml_correct_attr_ns(ctx, attr, elem_root, copy_ns);
            }
        }
        LY_TREE_DFS_END(elem, tmp, iter);
    }
}