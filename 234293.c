setGraphAttr(Agraph_t * g, char *name, char *value, userdata_t * ud)
{
    Agsym_t *ap;

    if ((g == root) && !strcmp(name, "strict") && !strcmp(value, "true")) {
	g->desc.strict = 1;
    } else if (strcmp(name, "name") == 0)
	setName(ud->nameMap, (Agobj_t *) g, value);
    else {
	ap = agattr(root, AGRAPH, name, 0);
	if (ap)
	    agxset(g, ap, value);
	else if (g == root)
	    agattr(root, AGRAPH, name, value);
	else {
	    ap = agattr(root, AGRAPH, name, defval);
	    agxset(g, ap, value);
	}
    }
}