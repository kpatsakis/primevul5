setGlobalNodeAttr(Agraph_t * g, char *name, char *value, userdata_t * ud)
{
    if (strncmp(name, NODELBL, NLBLLEN))
	fprintf(stderr,
		"Warning: global node attribute %s in graph %s does not begin with the prefix %s\n",
		name, agnameof(g), NODELBL);
    else
	name += NLBLLEN;
    if ((g != root) && !agattr(root, AGNODE, name, 0))
	agattr(root, AGNODE, name, defval);
    agattr(G, AGNODE, name, value);
}