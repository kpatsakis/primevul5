void dupattvalues(struct attvaluelist **dst,
                  const struct attvaluelist *src)
{
    for ( ; src ; src = src->next)
        appendattvalue(dst, src->attrib, &src->value);
}