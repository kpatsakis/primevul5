static void setName(Dt_t * names, Agobj_t * n, char *value)
{
    Agsym_t *ap;
    char *oldName;

    ap = agattr(root, AGTYPE(n), GRAPHML_ID, "");
    agxset(n, ap, agnameof(n));
    oldName = agxget(n, ap);	/* set/get gives us new copy */
    addToMap(names, oldName, value);
    agrename(n, value);
}