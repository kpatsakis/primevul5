setGlobalEdgeAttr(Agraph_t * g, char *name, char *value, userdata_t * ud)
{
    if (strncmp(name, EDGELBL, ELBLLEN))
	fprintf(stderr,
		"Warning: global edge attribute %s in graph %s does not begin with the prefix %s\n",
		name, agnameof(g), EDGELBL);
    else
	name += ELBLLEN;
    if ((g != root) && !agattr(root, AGEDGE, name, 0))
	agattr(root, AGEDGE, name, defval);
    agattr(g, AGEDGE, name, value);
}