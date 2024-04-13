EXPORTED void freestrlist(struct strlist *l)
{
    struct strlist *n;

    while (l) {
        n = l->next;
        free(l->s);
        if (l->p) charset_freepat(l->p);
        free((char *)l);
        l = n;
    }
}