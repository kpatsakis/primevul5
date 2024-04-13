EXPORTED void freeattvalues(struct attvaluelist *l)
{
    struct attvaluelist *n;

    while (l) {
        n = l->next;
        free(l->attrib);
        buf_free(&l->value);
        free(l);
        l = n;
    }
}