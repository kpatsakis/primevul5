EXPORTED void appendattvalue(struct attvaluelist **l,
                    const char *attrib,
                    const struct buf *value)
{
    struct attvaluelist **tail = l;

    while (*tail) tail = &(*tail)->next;

    *tail = xzmalloc(sizeof(struct attvaluelist));
    (*tail)->attrib = xstrdup(attrib);
    buf_copy(&(*tail)->value, value);
}