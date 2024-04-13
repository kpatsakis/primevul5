setNodeAttr(Agnode_t * np, char *name, char *value, userdata_t * ud)
{
    Agsym_t *ap;

    if (strcmp(name, "name") == 0) {
	setName(ud->nameMap, (Agobj_t *) np, value);
    } else {
	ap = agattr(root, AGNODE, name, 0);
	if (!ap)
	    ap = agattr(root, AGNODE, name, defval);
	agxset(np, ap, value);
    }
}