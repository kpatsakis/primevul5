lyxml_free_attr(struct ly_ctx *ctx, struct lyxml_elem *parent, struct lyxml_attr *attr)
{
    struct lyxml_attr *aiter, *aprev;

    if (!attr) {
        return;
    }

    if (parent) {
        /* unlink attribute from the parent's list of attributes */
        aprev = NULL;
        for (aiter = parent->attr; aiter; aiter = aiter->next) {
            if (aiter == attr) {
                break;
            }
            aprev = aiter;
        }
        if (!aiter) {
            /* attribute to remove not found */
            return;
        }

        if (!aprev) {
            /* attribute is first in parent's list of attributes */
            parent->attr = attr->next;
        } else {
            /* reconnect previous attribute to the next */
            aprev->next = attr->next;
        }
    }
    lydict_remove(ctx, attr->name);
    lydict_remove(ctx, attr->value);
    if (attr->type == LYXML_ATTR_STD_UNRES) {
        free((char *)attr->ns);
    }
    free(attr);
}