EXPORTED size_t sizeentryatts(const struct entryattlist *l)
{
    size_t sz = 0;
    struct attvaluelist *av;

    for ( ; l ; l = l->next)
        for (av = l->attvalues ; av ; av = av->next)
            sz += av->value.len;
    return sz;
}