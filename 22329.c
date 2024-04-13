EXPORTED void clearentryatt(struct entryattlist **l, const char *entry,
                   const char *attrib)
{
    struct entryattlist *ea, **pea;
    struct attvaluelist *av, **pav;

    for (pea = l ; *pea ; pea = &(*pea)->next) {
        if (!strcmp((*pea)->entry, entry))
            break;
    }
    ea = *pea;
    if (!ea)
        return; /* entry not found */

    for (pav = &(*pea)->attvalues ; *pav ; pav = &(*pav)->next) {
        if (!strcmp((*pav)->attrib, attrib))
            break;
    }
    av = *pav;
    if (!av)
        return; /* attrib not found */

    /* detach and free attvaluelist */
    *pav = av->next;
    free(av->attrib);
    buf_free(&av->value);
    free(av);

    if (!ea->attvalues) {
        /* ->attvalues is now empty, so we can detach and free *pea too */
        *pea = ea->next;
        free(ea->entry);
        free(ea);
    }
}