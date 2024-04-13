void dupentryatt(struct entryattlist **dst,
                 const struct entryattlist *src)
{
    for ( ; src ; src = src->next) {
        struct attvaluelist *attvalues = NULL;
        dupattvalues(&attvalues, src->attvalues);
        appendentryatt(dst, src->entry, attvalues);
    }
}