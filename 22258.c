EXPORTED void freeentryatts(struct entryattlist *l)
{
    struct entryattlist *n;

    while (l) {
        n = l->next;
        free(l->entry);
        freeattvalues(l->attvalues);
        free(l);
        l = n;
    }
}