int agdelete(Agraph_t * g, void *obj)
{
    if ((AGTYPE((Agobj_t *) obj) == AGRAPH) && (g != agparent(obj))) {
	agerr(AGERR, "agdelete on wrong graph");
	return FAILURE;
    }

    switch (AGTYPE((Agobj_t *) obj)) {
    case AGNODE:
	return agdelnode(g, obj);
    case AGINEDGE:
    case AGOUTEDGE:
	return agdeledge(g, obj);
    case AGRAPH:
	return agclose(obj);
    default:
	agerr(AGERR, "agdelete on bad object");
    }
    return SUCCESS;		/* not reached */
}