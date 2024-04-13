lyxml_get_attr(const struct lyxml_elem *elem, const char *name, const char *ns)
{
    FUN_IN;

    struct lyxml_attr *a;

    assert(elem);
    assert(name);

    for (a = elem->attr; a; a = a->next) {
        if (a->type != LYXML_ATTR_STD) {
            continue;
        }

        if (!strcmp(name, a->name)) {
            if ((!ns && !a->ns) || (ns && a->ns && !strcmp(ns, a->ns->value))) {
                return a->value;
            }
        }
    }

    return NULL;
}