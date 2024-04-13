setEdgeAttr(Agedge_t * ep, char *name, char *value, userdata_t * ud)
{
    Agsym_t *ap;
    char *attrname;

    if (strcmp(name, "headport") == 0) {
	if (ud->edgeinverted)
	    attrname = "tailport";
	else
	    attrname = "headport";
	ap = agattr(root, AGEDGE, attrname, 0);
	if (!ap)
	    ap = agattr(root, AGEDGE, attrname, defval);
	agxset(ep, ap, value);
    } else if (strcmp(name, "tailport") == 0) {
	if (ud->edgeinverted)
	    attrname = "headport";
	else
	    attrname = "tailport";
	ap = agattr(root, AGEDGE, attrname, 0);
	if (!ap)
	    ap = agattr(root, AGEDGE, attrname, defval);
	agxset(ep, ap, value);
    } else {
	ap = agattr(root, AGEDGE, name, 0);
	if (!ap)
	    ap = agattr(root, AGEDGE, name, defval);
	agxset(ep, ap, value);
    }
}