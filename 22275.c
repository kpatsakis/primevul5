EXPORTED void setentryatt(struct entryattlist **l, const char *entry,
                 const char *attrib, const struct buf *value)
{
    struct entryattlist *ee;

    for (ee = *l ; ee ; ee = ee->next) {
        if (!strcmp(ee->entry, entry))
            break;
    }

    if (!ee) {
        struct attvaluelist *atts = NULL;
        appendattvalue(&atts, attrib, value);
        appendentryatt(l, entry, atts);
    }
    else {
        struct attvaluelist *av;
        for (av = ee->attvalues ; av ; av = av->next) {
            if (!strcmp(av->attrib, attrib))
                break;
        }
        if (av)
            buf_copy(&av->value, value);
        else
            appendattvalue(&ee->attvalues, attrib, value);
    }
}