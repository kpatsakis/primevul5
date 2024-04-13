lyxml_get_ns(const struct lyxml_elem *elem, const char *prefix)
{
    FUN_IN;

    struct lyxml_attr *attr;

    if (!elem) {
        return NULL;
    }

    for (attr = elem->attr; attr; attr = attr->next) {
        if (attr->type != LYXML_ATTR_NS) {
            continue;
        }
        if (!attr->name) {
            if (!prefix) {
                /* default namespace found */
                if (!attr->value) {
                    /* empty default namespace -> no default namespace */
                    return NULL;
                }
                return (struct lyxml_ns *)attr;
            }
        } else if (prefix && !strcmp(attr->name, prefix)) {
            /* prefix found */
            return (struct lyxml_ns *)attr;
        }
    }

    /* go recursively */
    return lyxml_get_ns(elem->parent, prefix);
}